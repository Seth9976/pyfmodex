/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

//////////////////////////////////////////////////////////////////////
//
// AkDeviceDeferredLinedUp.h
//
// Win32 Deferred Scheduler Device implementation.
// Requests to low-level are sent in a lined-up fashion.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AkDeviceDeferredLinedUp.h"
#include <AK/Tools/Common/AkAutoLock.h>
#include <AK/Tools/Common/AkInstrument.h>
#include <AK/Tools/Common/AkPlacementNew.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/Tools/Common/AkProfilingID.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>

using namespace AK;
using namespace AK::StreamMgr;

//--------------------------------------------------------------------
// Defines.
//--------------------------------------------------------------------

CAkDeviceDeferredLinedUp::CAkDeviceDeferredLinedUp(IAkLowLevelIOHook *in_pLowLevelHook) 
	: CAkDeviceBase(in_pLowLevelHook)
	, m_pLowLevelTransfersMem( NULL )
{
}

CAkDeviceDeferredLinedUp::~CAkDeviceDeferredLinedUp( )
{
	if (m_pLowLevelTransfersMem)
	{
		m_poolLowLevelTransfers.RemoveAll();
		AkFree(AkMemID_Streaming, m_pLowLevelTransfersMem);
	}
	m_poolLowLevelTransfers.Term();
}

AKRESULT CAkDeviceDeferredLinedUp::Init( 
	const AkDeviceSettings &    in_settings,
	AkDeviceID                  in_deviceID,
    bool                        in_bUseIOThread
	)
{
	if ( 0 == in_settings.uMaxConcurrentIO )
	{
		AKASSERT( !"Invalid number of concurrent IO tranfers" );
		return AK_InvalidParameter;
	}

	AKRESULT eResult = CAkDeviceBase::Init( in_settings, in_deviceID, in_bUseIOThread );
	if ( AK_Success == eResult )
	{
		// Cache all transfer objects needed.

		m_pLowLevelTransfersMem = (CAkLowLevelTransfer*)AkAlloc( AkMemID_Streaming, in_settings.uMaxConcurrentIO * sizeof( CAkLowLevelTransfer ) );
		if ( !m_pLowLevelTransfersMem )
			return AK_Fail;

		CAkLowLevelTransfer * pXferObj = m_pLowLevelTransfersMem;
		CAkLowLevelTransfer * pXferObjEnd = pXferObj + in_settings.uMaxConcurrentIO;
		do
		{
			AkPlacementNew( pXferObj ) CAkLowLevelTransfer();
			m_poolLowLevelTransfers.AddFirst( pXferObj++ );
		}
		while ( pXferObj < pXferObjEnd );
	}
	return eResult;
}

// This device's implementation of PerformIO(), called by the I/O thread.
void CAkDeviceDeferredLinedUp::PerformIO()
{
	AkInt32 iMaxQueueableTasks;
	{
		AkAutoLock<CAkLock> lock(m_lockTasksList);
		iMaxQueueableTasks = m_uMaxConcurrentIO - GetNumConcurrentIO();
		if (iMaxQueueableTasks <= 0)	//NumConcurrentIO can temporarily be over because of EnsureCachedTransfers.
		{
			//If we have no space to queue another I/O task, at least do some housekeeping.
			CleanDeadTasks(m_listTasks);
			CleanDeadTasks(m_listCachingTasks);
			return;
		}
	}

	AkInt32 iTotalTasksQueued = 0;

	// Determine list of tasks to be executed
	static_assert(CAkStmTask::OpRead == 0 && CAkStmTask::OpWrite == 1, "OpType should start with OpRead/OpWrite");
	AkUInt32 uNumTransfersQueued[2] = { 0, 0 };
	IAkLowLevelIOHook::BatchIoTransferItem* pBatchTransferInfo[2] =
	{
		(IAkLowLevelIOHook::BatchIoTransferItem*)AkAlloca(sizeof(IAkLowLevelIOHook::BatchIoTransferItem) * iMaxQueueableTasks),
		(IAkLowLevelIOHook::BatchIoTransferItem*)AkAlloca(sizeof(IAkLowLevelIOHook::BatchIoTransferItem) * iMaxQueueableTasks),
	};

	AkUInt32 uNumOpens = 0;
	AkAsyncFileOpenData** pBatchOpenList = (AkAsyncFileOpenData**)AkAlloca(sizeof(AkAsyncFileOpenData*) * iMaxQueueableTasks);
	{
		AK_INSTRUMENT_SCOPE_PROFILINGID(AK::ProfilingID::IODevice_TaskUpdate);
		CAkStmTask *pTask = NULL;
		float fOpDeadline = 0;
		while (iTotalTasksQueued < iMaxQueueableTasks)
		{
			AkAutoLock<CAkLock> lock(m_lockTasksList);

			pTask = SchedulerFindNextTask(fOpDeadline);
			if (pTask == nullptr)
			{
				break;
			}

			if (pTask->CurrentOp() == CAkStmTask::OpOpen)
			{
				AKRESULT res = pTask->PrepareOpen(pBatchOpenList[uNumOpens]);
				if (res != AK_Success || pBatchOpenList[uNumOpens] == nullptr || !pBatchOpenList[uNumOpens]->IsValid())
				{
					pTask->Update(NULL, AK_Cancelled, false);
					continue;
				}
				uNumOpens++;
				iTotalTasksQueued++;
			}
			else
			{
				// Get info for IO.
				AkFileDesc* pFileDesc;
				CAkLowLevelTransfer* pLowLevelXfer;
				bool bTransferExistsAlready;
				CAkStmMemView* pMemView;
				AKRESULT res = pTask->PrepareTransfer(pFileDesc, pLowLevelXfer, pMemView, bTransferExistsAlready, false);
				//PrepareTransfer can return pMemView = NULL for 2 reasons
				// - No more available memory
				// - The stream is at the end (no more bytes to read).
				if (!pMemView)
				{
					if (res == AK_InsufficientMemory)
					{
						if (pTask->StmType() == AK_StmTypeAutomatic && m_numPendingAutoStms > 1)
						{
							//So it means we lacked memory. This is not fatal, we can try to read later.
							NotifyMemFull();
							pTask->Update(NULL, AK_Success, false);
						}
						else
						{
							pTask->Update(NULL, AK_Fail, false);
						}
						continue;
					}

					AKASSERT(res == AK_InsufficientMemory || res == AK_Fail || res == AK_NoMoreData || !"Unsupported PrepareTransfer return code");
					pTask->Update(NULL, AK_Cancelled, false);
					continue;
				}
				
				if (!pLowLevelXfer)
				{
					if (!bTransferExistsAlready)
					{
						// Update task now if no transfer to the low-level IO was needed, and another transfer does not already exist.
						// If we grabbed a block from cache, and a transfer already exists, it means that this memory block is not yet ready.  It will
						// be updated in the callback from the LLIO.
						pTask->Update(pMemView, AK_Success, false);
					}

					// No lowlevel xfer otherwise, so skip this task
					continue;
				}

				CAkLowLevelTransfer* pLowLevelXferDeferred = (CAkLowLevelTransfer*)pLowLevelXfer;
				if (!pLowLevelXferDeferred->GetWasSentToLLIO())
				{
					pLowLevelXferDeferred->SetWasSentToLLIO();

					AkUInt32 uInfoArray = pTask->CurrentOp();
					AkUInt32 uIdx = uNumTransfersQueued[uInfoArray];
					uNumTransfersQueued[uInfoArray]++;
					pBatchTransferInfo[uInfoArray][uIdx].pFileDesc = pFileDesc;
					pBatchTransferInfo[uInfoArray][uIdx].ioHeuristics.priority = pTask->Priority();
					pBatchTransferInfo[uInfoArray][uIdx].ioHeuristics.fDeadline = fOpDeadline;
					pBatchTransferInfo[uInfoArray][uIdx].pTransferInfo = &(pLowLevelXferDeferred->info);

					iTotalTasksQueued++;
				}
			}
		}
	}

	IAkLowLevelIOHook* pIoHookDeferred = static_cast<IAkLowLevelIOHook*>(m_pLowLevelHook);
	if (uNumOpens)
	{
		AK_INSTRUMENT_SCOPE_PROFILINGID(AK::ProfilingID::IODevice_BatchOpen);
		pIoHookDeferred->BatchOpen(uNumOpens, pBatchOpenList);

		iTotalTasksQueued -= uNumOpens;
	}

	// Dispatch the deferred transfers
	if (iTotalTasksQueued > 0)
	{
		for (AkUInt32 uOpIdx = 0; uOpIdx < 2; ++uOpIdx)
		{
			AkUInt32 uNumTransfers = uNumTransfersQueued[uOpIdx];
			if (uNumTransfers == 0)
				continue;

			IAkLowLevelIOHook::BatchIoTransferItem* pTransferInfoList = pBatchTransferInfo[uOpIdx];
			if (uOpIdx == CAkStmTask::OpRead)
			{
				AK_INSTRUMENT_SCOPE_PROFILINGID(AK::ProfilingID::IODevice_BatchRead);
				pIoHookDeferred->BatchRead(uNumTransfers, pTransferInfoList);
			} 
			else
			{
				AK_INSTRUMENT_SCOPE_PROFILINGID(AK::ProfilingID::IODevice_BatchWrite);
				pIoHookDeferred->BatchWrite(uNumTransfers, pTransferInfoList);
			}
		}
	}
}

// IO memory access.
// -----------------------------------------------------

// Creates a view to the desired streaming memory, for standard streams.
// Accepts a memory block, which should map the user-provided memory.
// A view, pointing to this block, is created and returned. A new low-level transfer is created
// and attached to it, and is returned via out_pLowLevelXfer.
// If there was a failure (out of small object memory), the function returns NULL.
// Sync: 
//	- Thread safe. Internally locks device for memory access. 
//	- Client status should be locked prior to calling this function.
// Note: If the mem block is already busy, a new one is created temporarily.
CAkStmMemViewDeferred * CAkDeviceDeferredLinedUp::CreateMemViewStd(
	CAkStdStmDeferredLinedUp *	in_pOwner,	// Owner task (standard stream).
	AkMemBlock *	in_pMemBlock,		// Memory block for data view. 
	AkUInt32		in_uDataOffset,		// Data view offset from memory block start.
	AkUInt64		in_uPosition,		// Desired position in file.
	AkUInt32 		in_uBufferSize,		// Buffer size. Same as in_uRequestedSize except at EOF.
	AkUInt32 		in_uRequestedSize,	// Requested size.
	CAkLowLevelTransfer *& out_pLowLevelXfer	// Returned low-level transfer.
	)
{
	out_pLowLevelXfer = NULL;

	AkAutoLock<CAkDeviceBase> deviceLock( *this );

	// Create a new streaming memory view for this transfer.
	// If this fails, everything fails.
	CAkStmMemViewDeferred* pMemView = AkNew(AkMemID_Streaming, CAkStmMemViewDeferred(true));
	if (pMemView)
	{
		// Instantiate a new temporary memblock if this one is busy.
		AkMemBlock * pMemBlockForTransfer;
		if ( in_pMemBlock->IsBusy() )
		{
			m_mgrMemIO.CloneTempBlock( in_pMemBlock, pMemBlockForTransfer );
			if ( !pMemBlockForTransfer )
			{
				// Could not create a temporary block!
				// Get rid of mem view.
				DestroyMemView( pMemView );
				return NULL;
			}
		}
		else
			pMemBlockForTransfer = in_pMemBlock;


		// Create a low-level transfer (must succeed) and attach it to the memblock.
		out_pLowLevelXfer = CreateLowLevelTransfer( 
			in_pOwner,			// Owner task.
			(AkUInt8*)pMemBlockForTransfer->pData + in_uDataOffset, // Address for transfer.
			in_uPosition,		// Position in file, relative to start of file.
			in_uBufferSize,		// Buffer size.
			in_uRequestedSize	// Requested transfer size.
			);
		AKASSERT( out_pLowLevelXfer );	// Cannot fail.
		pMemBlockForTransfer->pTransfer = out_pLowLevelXfer;

		// Create a view to this memory block. The offset is the size that has been read already.
		pMemView->Attach( pMemBlockForTransfer, in_uDataOffset );

		// Add ourselves to low-level transfer's observers list if there is a transfer.
		out_pLowLevelXfer->AddObserver( pMemView );

		// Enqueue new transfer request.
		in_pOwner->PushTransferRequest( pMemView );
	}

	return pMemView;
}

// Creates a view to the desired streaming memory, for automatic streams.
// Searches a memory block for IO. If available, tries to get a buffer with cached data. 
// Otherwise, returns a new buffer for IO. 
// If a block is found, a view, pointing to this block, is created and returned.
// If the block requires a transfer, a new low-level transfer is created and attached to it. 
// If it is already busy, a low-level transfer is already attached to it. The returned memory view
// is added as an observer of that transfer.
// If the transfer needs to be pushed to the Low-Level IO, it is returned via out_pLowLevelXfer.
// If a block was not found, the function returns NULL.
// Sync: 
//	- Thread safe. Internally locks device for memory access. Notifies memory full if applicable, atomically.
//	- Client status should be locked prior to calling this function.
CAkStmMemViewDeferred * CAkDeviceDeferredLinedUp::CreateMemViewAuto(
	CAkAutoStmDeferredLinedUp *	in_pOwner,	// Owner task (automatic stream).
	AkCacheID		in_cacheID,			// Block's associated file ID.
	AkUInt64		in_uPosition,		// Desired position in file.
	AkUInt32		in_uMinSize,		// Minimum data size acceptable (discard otherwise).
	AkUInt32		in_uRequiredAlign,	// Required data alignment.
	bool			in_bEof,			// True if desired block is last of file.
	bool			in_bCacheOnly,		// Get a view of cached data only, otherwise return NULL.
	AkUInt32 &		io_uRequestedSize,	// In: desired data size; Out: returned valid size.
	CAkLowLevelTransfer *& out_pNewLowLevelXfer,	// Returned low-level transfer if it was created. Device must push it to the Low-Level IO. NULL otherwise.
	bool & out_bExistingLowLevelXfer // Set to true if a low level transfer already exists for the memory block that is referenced by the returned view.
	)
{
	out_pNewLowLevelXfer = NULL;
	out_bExistingLowLevelXfer = false;

	// I/O pool access must be enclosed in scheduler lock.
	AkAutoLock<CAkDeviceBase> deviceLock( *this );

	AkMemBlock * pMemBlock = NULL;

	// Try to get a block from the cache if in_fileID != AK_INVALID_CACHE_ID. The minimum
	// buffer size acceptable is equal to m_uMinBufferSize (user-specified buffer constraint), 
	// unless io_uRequestedSize is smaller (which typically occurs at the end of file).
	AkUInt32 uOffset = ( m_mgrMemIO.UseCache() && in_cacheID != AK_INVALID_CACHE_ID ) ? m_mgrMemIO.GetCachedBlock(
		in_cacheID,
		in_uPosition, 
		in_uMinSize,
		in_uRequiredAlign, 
		in_bEof,
		io_uRequestedSize, 
		pMemBlock ) : 0;

	if ( in_bCacheOnly ) //Requested a cache-only transfer. We got some special rules to follow here.
	{ 
		if ( pMemBlock == NULL )
		{
			return NULL;
		}
		else if ( pMemBlock->IsBusy() )
		{
			// If using cached data, we need to ensure that the block is not currently being filled up.
			m_mgrMemIO.ReleaseBlock( pMemBlock );
			return NULL;
		}
	}

	// Create a new streaming memory view for this transfer.
	// If this fails, everything fails.
	CAkStmMemViewDeferred * pMemView = AkNew(AkMemID_Streaming, CAkStmMemViewDeferred(true));
	if (pMemView)
	{
		if ( !pMemBlock )
		{
			// Nothing useful in cache. Acquire free buffer.
			AKASSERT( uOffset == 0 );
			m_mgrMemIO.GetOldestFreeBlock( io_uRequestedSize, in_uRequiredAlign, pMemBlock );
			if ( pMemBlock )
			{
				// Got a new block. Create a new low-level transfer and tag block.
				out_pNewLowLevelXfer = CreateLowLevelTransfer(
					in_pOwner, 
					pMemBlock->pData, 
					in_uPosition, 
					pMemBlock->uAllocSize, //Make sure to pass entire alloc size, which is block alligned.
					io_uRequestedSize 
					);
				AKASSERT(out_pNewLowLevelXfer);

				//NOTE:  Tagging the memory block can fail.  This will just mean that we can not re-use the block in cache.
				m_mgrMemIO.TagBlock( 
									pMemBlock, 
									out_pNewLowLevelXfer,
									in_cacheID, 
									in_uPosition, 
									io_uRequestedSize 
									);

				// Memory block obtained requires an IO transfer. Effective address points 
				// at the beginning of the block. io_uRequestedSize is unchanged.
				// Create a view to this memory block and add ourselves to low-level as the first transfer's observers.
				pMemView->Attach( pMemBlock, 0 );
				out_pNewLowLevelXfer->AddObserver(pMemView);
			}
			else
			{
				// No memory. Get rid of mem view and notify out of memory.
				DestroyMemView( pMemView );
				return NULL;
			}
		}
		else
		{
			// Using cache.
			// Create a view to this memory block and add ourselves to low-level transfer's observers list 
			// if there is a transfer.
			pMemView->Attach( pMemBlock, uOffset );
			CAkLowLevelTransfer* pLowLevelXfer = (CAkLowLevelTransfer*)pMemBlock->pTransfer;
			if ( pLowLevelXfer )
			{
				//NOTE: *do not* pass out pLowLevelXfer.  This transfer is in use, and can be deleted at any time after the lock is released.
				//	Instead we return a boolean indicating that the transfer exists and will be updated when it is completed.
				pLowLevelXfer->AddObserver( pMemView );
				out_bExistingLowLevelXfer = true;
			}
			
		}

		// Enqueue new transfer request.
		in_pOwner->PushTransferRequest( pMemView );
	}
	
	return pMemView;
}


//-----------------------------------------------------------------------------
// Stream objects specificity.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: class CAkStdStmDeferredLinedUp
// Desc: Overrides methods for deferred lined-up device specificities.
//-----------------------------------------------------------------------------
CAkStdStmDeferredLinedUp::CAkStdStmDeferredLinedUp()
{
}

CAkStdStmDeferredLinedUp::~CAkStdStmDeferredLinedUp()
{
}

void CAkStdStmDeferredLinedUp::Cancel()
{
	AkAutoLock<CAkLock> stmGate(m_lockStatus);
		
	//Cancel unscheduled Write operations
	FreeWriteBuffers();
		
	// Set cancel state first
	SetStatus(AK_StmStatusCancelled, AK_Success);

	if (!IsFileOpened())
		return;

	// Note: rely on the pending transfers list instead of the status ("idle" is used to
	// avoid waking up the scheduler; it does not mean that there is no transfer in progress).
	if (_CancelAllPendingTransfers())
		SetBlockedStatus();
}

// Info access.
// Sync: None. Status query.
// Override GetStatus(): return "pending" if we are in fact Idle, but transfers are pending.
AkStmStatus CAkStdStmDeferredLinedUp::GetStatus()           // Get operation status.
{
	AkAutoLock<CAkLock> status( m_lockStatus );
	if (m_eStmStatus != AK_StmStatusError && m_eStmStatus != AK_StmStatusCancelled)
	{
		if (!m_listPendingXfers.IsEmpty() || m_pWriteBlocks || m_pLastBlock)
			return AK_StmStatusPending;
	}
    return m_eStmStatus;
}

//MUST be locked from the caller. Note the reverse lock because both m_eOp and m_pLastBlock must be protected.
//Note: yes, it looks weird, but this code repeats and it is more readable with this function.
//This differs from WaitForPendingOperation by a) waiting only for Write ops and b) not getting in/out of the lock when not needed.
void CAkStdStmDeferredLinedUp::FlushWritesAndWait(bool in_bWait)
{
	if (m_eStmStatus != AK_StmStatusError && m_eStmStatus != AK_StmStatusCancelled && (m_pLastBlock || m_pWriteBlocks))
	{
		//Properly flush the last write to disk.
		if (m_pLastBlock)
			QueueWrite();	//Errors will set status to AK_StmStatusError

		if (in_bWait && GetStatus() == AK_StmStatusPending)
		{
			SetBlockedStatus();
			m_lockStatus.Unlock();
			m_pDevice->WaitForIOCompletion(this);
			m_lockStatus.Lock();
		}
	}
}

void CAkStdStmDeferredLinedUp::FreeWriteBuffers()
{
	while (m_pWriteBlocks)
	{
		AkMemBlock* pNext = m_pWriteBlocks->pNextBlock;
		AkFree(AkMemID_Streaming, m_pWriteBlocks);
		m_pWriteBlocks = pNext;
	}

	if (m_pLastBlock)
		AkFree(AkMemID_Streaming, m_pLastBlock);
	m_pLastBlock = nullptr;
}

// Override destroy:
// - Cancel pending transfers
// - Lock all operations with scheduler and status locks, in the correct order.
void CAkStdStmDeferredLinedUp::Destroy()
{
	m_lockStatus.Lock();
	if (!m_pDevice)
	{
		SetToBeDestroyed();
		m_lockStatus.Unlock();

		InstantDestroy();
		return;
	}

	FlushWritesAndWait(true);
	FreeWriteBuffers();

	bool bWaitForCompletion = false;

	m_eStmStatus = AK_StmStatusCancelled;
	
	// If an operation is pending, the scheduler might be executing it. This method must not return until it 
	// is complete: lock I/O for this task.
	// Cancel all remaining tasks.
	if (_CancelAllPendingTransfers())
	{
		SetBlockedStatus();
		bWaitForCompletion = true;
	}

	SetReadyForIO(false);

	m_lockStatus.Unlock();

	//Waiting is absolutely necessary if we cancelled tasks. The memory we write into
	//belongs to the stream client. Therefore it must stay valid until the reads complete.
	if (bWaitForCompletion)
		m_pDevice->WaitForIOCompletion(this);

	m_lockStatus.Lock();
	SetToBeDestroyed();
	m_pDevice->NotifyMemChange();	//Force cleanup.
	m_lockStatus.Unlock();
}

// Get information for data transfer.
// Returns the (device-specific) streaming memory view containing logical transfer information. 
// NULL if preparing transfer has aborted.
// out_pLowLevelXfer is set if and only if the transfer requires a transfer in the low-level IO.
// Sync: Locks stream's status.
AKRESULT CAkStdStmDeferredLinedUp::PrepareTransfer( 
	AkFileDesc *&			out_pFileDesc,		// Stream's associated file descriptor.
	CAkLowLevelTransfer *&	out_pLowLevelXfer,	// Low-level transfer. Set to NULL if it doesn't need to be pushed to the Low-Level IO.
	CAkStmMemView *&		out_pMemView,		// Memory space to fill
	bool &					out_bExistingLowLevelXfer, // Always false for std streams.
	bool					in_bCacheOnly		// NOT Supported.
	)
{
	AKASSERT( !in_bCacheOnly || !"Not supported" );

	out_pLowLevelXfer = nullptr;
	out_bExistingLowLevelXfer = false;
	out_pMemView = nullptr;

	// Lock status.
    AkAutoLock<CAkLock> atomicPosition( m_lockStatus );

	// From here on, Update() will have to be called in order to clean this transfer, whether it actually happens or not. 
	// IO count is decremented in Update().
	m_pDevice->IncrementIOCount();

	// Status is locked: last chance to bail out if the stream was destroyed by client.
	if ( m_bIsToBeDestroyed )
		return AK_Fail;

	out_pFileDesc = m_pFileDesc;

	if (CurrentOp() == OpRead)
	{
		AkUInt64 uFilePosition = GetCurUserPosition() + m_uTotalScheduledSize;

		// Required transfer size is the buffer size for this stream.
		// Slice request to granularity.
		// Cannot overshoot client buffer.
		// NOTE: m_memBlock.uAvailableSize is the original client request size.
		AKASSERT(m_uTotalScheduledSize <= m_memBlock.uAvailableSize);
		AkUInt32 uMaxTransferSize = (m_memBlock.uAvailableSize - m_uTotalScheduledSize);
		if (uMaxTransferSize > m_pDevice->GetGranularity())
			uMaxTransferSize = m_pDevice->GetGranularity();

		// Clamp request size to eof if reading.
		bool bWillReachEof = false;
		AkUInt32 uRequestedSize = (CurrentOp() == OpRead) ? ClampRequestSizeToEof(uFilePosition, uMaxTransferSize, bWillReachEof) : uMaxTransferSize;

		// Create a new memory view for transfer, and attach a low-level transfer to our mem block.
		// If our mem block is already busy, a new one is created inside CreateMemViewStd().
		out_pMemView = ((CAkDeviceDeferredLinedUp*)m_pDevice)->CreateMemViewStd(
			this,						// Owner.
			&m_memBlock,				// Memory block (base address).
			m_uTotalScheduledSize,		// Offset.
			uFilePosition,				// Position in file.
			uMaxTransferSize,			// Buffer size. 
			uRequestedSize,				// Requested size.
			out_pLowLevelXfer			// Returned low level transfer (always set if successful: standard stream cannot use cached data).
		);

		if (!out_pMemView)
			return AK_InsufficientMemory;

		// Check if client request will complete after this transfer.
		m_uTotalScheduledSize += uRequestedSize;
		if (bWillReachEof || m_uTotalScheduledSize == m_memBlock.uAvailableSize)
		{
			// Yes. Set as "idle", in order to stop this stream from being scheduled for I/O.
			SetStatus(AK_StmStatusIdle, AK_Success);
		}
	}
	else
	{
		// Create a new memory view for transfer, and attach a low-level transfer to our mem block.
		AkMemBlock * pNext = m_pWriteBlocks->pNextBlock;	//Will be overwritten.
		out_pMemView = ((CAkDeviceDeferredLinedUp*)m_pDevice)->CreateMemViewStd(
			this,						// Owner.
			m_pWriteBlocks,				// Memory block (base address).
			0,							// No Offset, write from begining of buffer
			m_pWriteBlocks->uPosition,	// Position in file.
			m_pWriteBlocks->uAllocSize,	// Buffer size. 
			m_pWriteBlocks->uAvailableSize,	// Requested size.
			out_pLowLevelXfer				// Returned low level transfer (always set if successful: standard stream cannot use cached data).
		);

		if (!out_pMemView)
		{
			//No memory? Don't touch the m_pWriteBlocks and wait a cycle.
			AKASSERT(m_pWriteBlocks->pNextBlock == pNext);
			return AK_InsufficientMemory;
		}

		m_uTotalScheduledSize -= m_pWriteBlocks->uAvailableSize;
		m_pWriteBlocks = pNext;

		//Don't send multiple Write for the same file to the LLIO at the same time. 
		//Writes are less important than reads, so don't use up more than 1 transfer slot per write file.
		//See AddMemView for counterpart
		SetReadyForIO(false);
	}

	if (out_pLowLevelXfer)
		out_pLowLevelXfer->info.pUserData = m_pUserData;
		
	// Reset timer. Time count since last transfer starts now.
    m_iIOStartTime = m_pDevice->GetTime();

	return AK_Success;
}

// Cancel all pending transfers.
// Stream must be locked.
bool CAkStdStmDeferredLinedUp::_CancelAllPendingTransfers()
{
	// Cancel all transfers of the pending transfers list.
	// Pass true for flag "all cancelled": there is no caching with standard stream, and we want to cancel all transfers for this stream.
	return CancelTransfers( m_listPendingXfers );
}

AKRESULT CAkStdStmDeferredLinedUp::Read(
	void* in_pBuffer,			// User buffer address. 
	AkUInt32	in_uReqSize,		// Requested read size.
	bool		in_bWait,			// Block until operation is complete.
	AkPriority	in_priority,		// Heuristic: operation priority.
	AkReal32	in_fDeadline,		// Heuristic: operation deadline (s).
	AkUInt32& out_uSize)			// Size actually read.
{
	//In this implementation we can't have pending writes and reads at the same time. Writes must be completed first.
	//This is a "test only" scenario. Wwise currently has no use for such pattern yet.
	if (m_eOp == OpWrite)
	{
		AkAutoLock<CAkLock> status(m_lockStatus);
		FlushWritesAndWait(true);
	}

	return CAkStmDeferredLinedUpBase<CAkStdStmBase>::Read(in_pBuffer, in_uReqSize, in_bWait, in_priority, in_fDeadline, out_uSize);
}

AKRESULT CAkStdStmDeferredLinedUp::Write(
	void* in_pBuffer,			// User buffer address. 
	AkUInt32		in_uReqSize,		// Requested write size. 
	bool			in_bWait,			// Block until operation is complete.
	AkPriority		in_priority,		// Heuristic: operation priority.
	AkReal32		in_fDeadline,		// Heuristic: operation deadline (s).
	AkUInt32& out_uSize			// Size actually written.
)
{
	out_uSize = 0;

	// Check requested size.
	if (in_pBuffer == NULL)
	{
		char err_msg[100];
		AKPLATFORM::SafeStrFormat(err_msg, sizeof(err_msg), "%s, invalid buffer", __func__);
		AK::Monitor::PostString(err_msg, AK::Monitor::ErrorLevel_Error);
		return AK_InvalidParameter;
	}

	// Check heuristics.
	if (in_priority < AK_MIN_PRIORITY ||
		in_priority > AK_MAX_PRIORITY ||
		in_fDeadline < 0)
	{
		char err_msg[100];
		AKPLATFORM::SafeStrFormat(err_msg, sizeof(err_msg), "%s, Invalid heuristics", __func__);
		AK::Monitor::PostString(err_msg, AK::Monitor::ErrorLevel_Error);
		return AK_InvalidParameter;
	}

	if (m_eOpenMode == AK_OpenModeRead)
	{
		AkAutoLock<CAkLock> status(m_lockStatus);
		SetStatus(AK_StmStatusError, AK_FilePermissionError);
		return AK_FilePermissionError;
	}

	// Check status.
	if (m_eStmStatus == AK_StmStatusError)
		return AK_Fail;

	// Leave right away if requested size turns out to be 0 (only for Read).
	if (0 == in_uReqSize)
		return AK_Success;

	//In this implementation we can't have pending writes and reads at the same time. Reads must be completed first.
	//This is a "test only" scenario. Wwise currently has no use for such pattern yet.
	if (m_eOp == OpRead && m_eStmStatus == AK_StmStatusPending)
		WaitForPendingOperation();

	{
		AkAutoLock<CAkLock> status(m_lockStatus);

		m_bIsOpComplete = false;

		if (m_eOp != OpWrite)
		{
			m_uTotalScheduledSize = 0;
			m_memBlock.uAvailableSize = 0;
		}

		m_eOp = OpWrite;
		m_priority = in_priority;
		m_fDeadline = in_fDeadline;

		//Need to copy the data, as we do not know the lifetime of the user data.
		//Also, we always buffer the data until we have enough to write.
		AkUInt32 uMaxTransfer = AkMax(16 * 1024, m_pDevice->GetGranularity());

		do
		{
			if (m_pLastBlock && m_pLastBlock->uAvailableSize + in_uReqSize > uMaxTransfer)
			{
				AkUInt32 uRemain = uMaxTransfer - m_pLastBlock->uAvailableSize;
				if (uRemain > 0)
				{
					memcpy((AkUInt8*)m_pLastBlock->pData + m_pLastBlock->uAvailableSize, in_pBuffer, uRemain);
					m_pLastBlock->uAvailableSize += uRemain;
					m_memBlock.uPosition += uRemain;	//Track the position in the block shared with read operations
					out_uSize += uRemain;
					in_uReqSize -= uRemain;
					in_pBuffer = (AkUInt8*)in_pBuffer + uRemain;
				}

				QueueWrite();
			}

			if (!m_pLastBlock)
			{
				m_pLastBlock = (AkMemBlock*)AkAlloc(AkMemID_Streaming, uMaxTransfer + sizeof(AkMemBlock));	//Allocate the struct and the data buffer together.
				if (!m_pLastBlock)
				{
					SetStatus(AK_StmStatusError, AK_InsufficientMemory);
					return AK_InsufficientMemory;
				}

				AkPlacementNew(m_pLastBlock) AkMemBlock(m_pLastBlock + 1);
				m_pLastBlock->uAllocSize = uMaxTransfer;
				m_pLastBlock->uPosition = m_memBlock.uPosition;
			}

			AkUInt32 uWrote = AkMin(uMaxTransfer, in_uReqSize);
			memcpy((AkUInt8*)m_pLastBlock->pData + m_pLastBlock->uAvailableSize, in_pBuffer, uWrote);

			m_pLastBlock->uAvailableSize += uWrote;
			m_memBlock.uPosition += uWrote;	//Track the position in the block shared with read operations
			out_uSize += uWrote;
			in_uReqSize -= uWrote;
			in_pBuffer = (AkUInt8*)in_pBuffer + uWrote;
		} while (in_uReqSize > 0);

		if (m_eStmStatus != AK_StmStatusPending)
			m_eStmStatus = AK_StmStatusIdle;
	}

	if (in_bWait)
		return WaitForPendingOperation() == AK_StmStatusError ? AK_Fail : AK_Success;

	return AK_Success;
}

void CAkStdStmDeferredLinedUp::FlushWrites(bool in_bWait)
{
	AkAutoLock<CAkLock> status(m_lockStatus);
	FlushWritesAndWait(in_bWait);
}

void CAkStdStmDeferredLinedUp::QueueWrite()
{
	if (GetStatus() != AK_StmStatusError)
	{
		//	AkAutoLock<CAkLock> status(m_lockStatus); Must be already locked by caller
		AKASSERT(m_pLastBlock);

		//Enough buffering, time to flush the buffer to disk.
		if (!m_pWriteBlocks)
		{
			m_pWriteBlocks = m_pLastBlock;
		}
		else
		{
			AkMemBlock* pCur = m_pWriteBlocks;
			while (pCur->pNextBlock)
				pCur = pCur->pNextBlock;

			pCur->pNextBlock = m_pLastBlock;
		}

		m_uTotalScheduledSize += m_pLastBlock->uAvailableSize;
		m_pLastBlock = nullptr;

		// Reset time.
		AKPLATFORM::PerformanceCounter(&m_iIOStartTime);

		SetStatus(AK_StmStatusPending, AK_Success);
	}
	else
	{
		FreeWriteBuffers();
	}
}

//Called when the transfer completes.
//For a Read operation, that means adding a memory view.
//For a Write operation, it means freeing up written memory blocks.
void CAkStdStmDeferredLinedUp::AddMemView(CAkStmMemView* in_pMemView, bool in_bStoreData)
{
	AkAutoLock<CAkDeviceBase> lock(*m_pDevice);

	CAkStmMemViewDeferred* pMemView = (CAkStmMemViewDeferred*)in_pMemView;
	if (m_eOp != OpWrite)
	{
		CAkStdStmBase::AddMemView(in_pMemView, in_bStoreData);
		return;
	}

	AkUInt32 uTransferSize = pMemView->Block()->uAvailableSize;

	m_bIsOpComplete = m_pWriteBlocks == nullptr && m_listPendingXfers.IsEmpty();

	//Don't send multiple Write for the same file to the LLIO at the same time. 
	//Writes are less important than reads, so don't use up more than 1 transfer slot per write file.
	//If there are still write blocks, put the stream as Ready again.
	//See PrepareTransfer for counterpart
	if (!m_bIsOpComplete)
	{
		SetReadyForIO(true);
		AKASSERT(m_eStmStatus == AK_StmStatusPending);
	}

	// Profiling.
#ifndef AK_OPTIMIZED
	m_uBytesTransfered += uTransferSize;

	AKASSERT(in_pMemView->Status() != CAkStmMemView::TransferStatus_Ready
		|| !"Standard streams cannot use cache data");
	if (m_pDevice->IsMonitoring())
		m_pDevice->PushTransferStatistics(uTransferSize, true);
#endif

	AkFree(AkMemID_Streaming, pMemView->Block());
	m_pDevice->DestroyMemView(pMemView->Block(), in_pMemView);
}

AkReal32 CAkStdStmDeferredLinedUp::EffectiveDeadline()
{
	if (CurrentOp() == OpOpen)
		return 0.f;

	AkUInt32 uGranularity = m_pDevice->GetGranularity();
	AkUInt32 uNumTransfersRemaining = 0;
	if (CurrentOp() == OpWrite)
		uNumTransfersRemaining = (m_uTotalScheduledSize + uGranularity - 1) / uGranularity;
	else
		uNumTransfersRemaining = (m_memBlock.uAvailableSize - m_uTotalScheduledSize + uGranularity - 1) / uGranularity;

	AKASSERT(uNumTransfersRemaining > 0);
	AkReal32 fDeadline = (m_fDeadline / uNumTransfersRemaining) - AKPLATFORM::Elapsed(m_pDevice->GetTime(), m_iIOStartTime);
	return (fDeadline > 0 ? fDeadline : 0);
}

AkStmStatus CAkStdStmDeferredLinedUp::WaitForPendingOperation()
{
	m_lockStatus.Lock();
	if (m_pLastBlock)
		QueueWrite();	//Errors will set status to AK_StmStatusError

	if (GetStatus() == AK_StmStatusPending)
	{
		SetBlockedStatus();
		m_lockStatus.Unlock();
		m_pDevice->WaitForIOCompletion(this);
	}
	else
	{
		m_lockStatus.Unlock();
	}
	return GetStatus();
}


void CAkStdStmDeferredLinedUp::OnFileDeferredOpen(const AkAsyncFileOpenData& in_OpenData, AKRESULT in_eResult)
{
	m_eStmStatus = AK_StmStatusIdle; // SetPosition wants this.
	if (in_eResult == AK_Success)
	{
		if (m_uNextExpectedUserPosition > 0)
			in_eResult = SetPosition(m_uNextExpectedUserPosition, AkMoveMethod::AK_MoveBegin);
	}

	AkStmStatus eStatus;
	if (in_eResult == AK_Success)
		eStatus = IsBlocked() ? GetStatus() : AK_StmStatusIdle;	//Status can still be pending if Writes have been queued after async open.
	else
		eStatus = AK_StmStatusError;

	SetStatus(eStatus, in_eResult);
	if (eStatus != AK_StmStatusPending)
		m_pDevice->SignalIOCompleted(this);
}


//-----------------------------------------------------------------------------
// Name: class CAkAutoStmDeferredLinedUp
// Desc: Base automatic stream implementation.
//-----------------------------------------------------------------------------
CAkAutoStmDeferredLinedUp::CAkAutoStmDeferredLinedUp()
{
}

CAkAutoStmDeferredLinedUp::~CAkAutoStmDeferredLinedUp()
{
}

// Get information for data transfer.
// Returns the (device-specific) streaming memory view containing logical transfer information. 
// NULL if preparing transfer has aborted.
// out_pLowLevelXfer is set if and only if the transfer requires a transfer in the low-level IO.
// Sync: Locks stream's status.
AKRESULT CAkAutoStmDeferredLinedUp::PrepareTransfer( 
	AkFileDesc *&			out_pFileDesc,		// Stream's associated file descriptor.
	CAkLowLevelTransfer *&	out_pNewLowLevelXfer,	// Low-level transfer. Set to NULL if it doesn't need to be pushed to the Low-Level IO.
	CAkStmMemView*&			out_pMemView,			// Memory space to fill
	bool				&	out_bExistingLowLevelXfer, // Indicates a low level transfer already exists that references the memory block returned.
	bool					in_bCacheOnly
	)
{
	out_pNewLowLevelXfer = nullptr;
	out_bExistingLowLevelXfer = false;
	out_pFileDesc = m_pFileDesc;
	out_pMemView = nullptr;

    // Lock status.
    AkAutoLock<CAkLock> atomicPosition( m_lockStatus );

	// From here on, Update() will have to be called in order to clean this transfer, whether it actually happens or not. 
	// IO count is decremented in Update().
	m_pDevice->IncrementIOCount();

	// Status is locked: last chance to bail out if the stream was destroyed by client.
	if ( m_bIsToBeDestroyed )
		return AK_Fail;

	// Get position for next transfer.
	AkUInt64 uFilePosition;
	AkUInt32 uRequestedSize;
	bool bEof;
	GetPositionForNextTransfer( uFilePosition, uRequestedSize, bEof );

	//Is is possible for caching streams to be exactly at the end of their prefetch buffer.
	if (uRequestedSize == 0)
	{
		SetReachedEof(bEof);
		return AK_NoMoreData;
	}

	// Get IO buffer.
	CAkLowLevelTransfer * pNewLowLevelXfer;
	out_pMemView = ((CAkDeviceDeferredLinedUp*)m_pDevice)->CreateMemViewAuto(
		this,					// Owner task.
		m_cacheID,				// File ID (for cache)
		uFilePosition,			// Desired position in file.
		AkMin( m_bufSettings.uMinBufferSize, uRequestedSize ), // Minimum data size acceptable is min between desired size and buffer constraint.
		m_uBufferAlignment,		// Required data alignment.
		bEof,					// True if desired block is last of file.
		in_bCacheOnly,			// Get a view of cached data only, otherwise return NULL.
		uRequestedSize,			// In: Desired data size. Out: Valid data size (may be smaller than input if using cache).
		pNewLowLevelXfer,			// Returned low-level transfer if a new one was created and it needs to be pushed to the Low-Level IO.
		out_bExistingLowLevelXfer	// Set to true if a low level transfer already exists.
		);
	if (!out_pMemView)
		return AK_InsufficientMemory; 

	out_pNewLowLevelXfer = pNewLowLevelXfer;
	if (out_pNewLowLevelXfer)
		pNewLowLevelXfer->info.pUserData = m_pUserData;

	// m_uVirtualBufferingSize takes looping heuristics into account. Clamp uRequestedSize if applicable.
	// When looping heuristics change, m_uVirtualBufferingSize is recomputed.
	if ( uFilePosition < m_uLoopEnd && ( uFilePosition + uRequestedSize ) > m_uLoopEnd )
		uRequestedSize = (AkUInt32)(m_uLoopEnd - uFilePosition);
	m_uVirtualBufferingSize += uRequestedSize;

	UpdateSchedulingStatus();

	// Reset timer. Time count since last transfer starts now.
	m_iIOStartTime = m_pDevice->GetTime();

	return AK_Success;
}

// Automatic streams must implement a method that returns the file position after the last
// valid (non cancelled) pending transfer. If there is no transfer pending, then it is the position
// at the end of buffering.
AkUInt64 CAkAutoStmDeferredLinedUp::GetVirtualFilePosition()
{
	// Must be locked.

	// Find the most recent transfer that was not cancelled (can be pending or completed).
	if ( m_listPendingXfers.Last() )
		return m_listPendingXfers.Last()->EndPosition();

	// Could not find a pending transfer that was not cancelled. Check in our list of buffers.
	if ( m_listBuffers.Length() > m_uNextToGrant )
		return ( m_listBuffers.Last()->EndPosition() );
	else
		return m_uNextExpectedFilePosition;	
}

// Cancel all pending transfers.
// Stream must be locked.
void CAkAutoStmDeferredLinedUp::CancelAllPendingTransfers()
{
	// Cancel all transfers of the pending transfers list.
	CancelTransfers( m_listPendingXfers );
}

// Cancel all pending transfers that are inconsistent with the next expected position (argument) 
// and looping heuristics. 
// Stream must be locked.
void CAkAutoStmDeferredLinedUp::CancelInconsistentPendingTransfers(
	AkUInt64 in_uNextExpectedPosition	// Expected file position of next transfer.
	)
{
	// Iterate through pending transfers, and dequeue any of them that is not 
	// consistent with the next expected position. Store them temporarily in a separate queue in order
	// to cancel them all at once.
	AkStmMemViewList listToCancel;
	AkStmMemViewList::IteratorEx it = m_listPendingXfers.BeginEx();
	while ( it != m_listPendingXfers.End() )
	{
		CAkStmMemView * pTransfer = *it;
		AKASSERT( pTransfer->Status() != CAkStmMemView::TransferStatus_Cancelled
				|| !"A cancelled transfer is in the pending queue" );

		if ( pTransfer->StartPosition() != in_uNextExpectedPosition )
		{
			// Dequeue and add to listToRemove.
			it = m_listPendingXfers.Erase( it );
			listToCancel.AddFirst( pTransfer );
		}
		else
		{
			// Valid. Keep it and update uNextExpectedPosition.
			in_uNextExpectedPosition = pTransfer->EndPosition();
			if ( m_uLoopEnd > 0 && in_uNextExpectedPosition >= m_uLoopEnd )
				in_uNextExpectedPosition = m_uLoopStart;
			++it;
		}
	}

	CancelTransfers( listToCancel );

	listToCancel.Term();
}

void CAkAutoStmDeferredLinedUp::FlushSmallBuffersAndPendingTransfers( 
	AkUInt32 in_uMinBufferSize 
	)
{
	bool bFlush = false;

	if ( m_listBuffers.Length() > m_uNextToGrant )
    {
		AkUInt32 uIdx = 0;
		AkBufferList::IteratorEx it = m_listBuffers.BeginEx();
		while ( uIdx < m_uNextToGrant )
	    {
			++uIdx;
			++it;
		}

		// Lock scheduler for memory change.
		{
			AkAutoLock<CAkDeviceBase> lock( *m_pDevice );

			while ( it != m_listBuffers.End() )
			{
				if ( bFlush 
					|| (*it)->Size() < in_uMinBufferSize )
				{
					bFlush = true;	// From now on, flush everything.

					CAkStmMemView * pMemView = (*it);
					it = m_listBuffers.Erase( it );
					DestroyBuffer( pMemView );
				}
				else
					++it;
			}
		}
	}

	// Cancel pending transfers if applicable.
	{
		// Try cancelling all transfers if some data has been flushed and caching is not enabled.
		{
			AkStmMemViewList::IteratorEx it = m_listPendingXfers.BeginEx();

			// Skip transfers that should be kept.
			if ( !bFlush )
			{
				while ( it != m_listPendingXfers.End() )
				{
					if ( (*it)->Size() < in_uMinBufferSize )
						break;
					++it;
				}
			}

			// (it) now points to the first transfer that should be cancelled. Cancel all the rest.

			while ( it != m_listPendingXfers.End() )
			{
				CAkStmMemView * pTransfer = (*it);
				AKASSERT( pTransfer->Status() != CAkStmMemView::TransferStatus_Cancelled
						|| !"A cancelled transfer is in the pending queue" );
				it = m_listPendingXfers.Erase( it );

				if ( pTransfer->Status() == CAkStmMemView::TransferStatus_Pending )
				{
					AddToCancelledList( pTransfer );
				}
				else
				{
					CancelCompleted( pTransfer );
				}
			}
		}

		{
			AkStmMemViewListLight::Iterator it = m_listCancelledXfers.Begin();
			while ( it != m_listCancelledXfers.End() )
			{
				// IMPORTANT: Cache reference before calling CancelTransfer because it could be dequeued 
				// inside this function: The Low-Level IO can call Update() directly from this thread.
				CAkStmMemViewDeferred * pTransfer = (CAkStmMemViewDeferred*)(*it);
				++it;
				pTransfer->Cancel();
			}
		}
	}
}

AkUInt32 CAkAutoStmDeferredLinedUp::ReleaseCachingBuffers(AkUInt32 in_uTargetMemToRecover)
{
	AkUInt32 uMemFreed = 0;

	{
		CAkStmMemView * pTransfer = m_listPendingXfers.Last();
		while (pTransfer != NULL && uMemFreed < in_uTargetMemToRecover) 
		{
			AKASSERT( pTransfer->Status() != CAkStmMemView::TransferStatus_Cancelled
				|| !"A cancelled transfer is in the pending queue" );

			uMemFreed += pTransfer->Size();
			AKVERIFY( m_listPendingXfers.Remove( pTransfer ) );

			if ( pTransfer->Status() == CAkStmMemView::TransferStatus_Pending )
			{
				AddToCancelledList( pTransfer );
			}
			else
			{
				CancelCompleted( pTransfer );
			}

			pTransfer = m_listPendingXfers.Last();
		}
	}

	//In case there were some remaining transfers in the pending list that have been completed out of order.
	UpdateCompletedTransfers();

	// Try to release more memory from the list of buffers by using the base class implementation
	uMemFreed += CAkAutoStmBase::ReleaseCachingBuffers(in_uTargetMemToRecover-uMemFreed);

	{
		AkStmMemViewListLight::Iterator it = m_listCancelledXfers.Begin();
		while ( it != m_listCancelledXfers.End() )
		{
			// IMPORTANT: Cache reference before calling CancelTransfer because it could be dequeued 
			// inside this function: The Low-Level IO can call Update() directly from this thread.
			CAkStmMemViewDeferred * pTransfer = (CAkStmMemViewDeferred*)(*it);
			++it;
			pTransfer->Cancel();
		}
	}

	return uMemFreed;
}

// Change loop end heuristic. Use this function instead of setting m_uLoopEnd directly because
// m_uLoopEnd has an impact on the computation of the effective data size (see GetEffectiveViewSize()).
// Implemented in derived classes because virtual buffering has to be recomputed.
void CAkAutoStmDeferredLinedUp::SetLoopEnd( 
	AkUInt64 in_uLoopEnd	// New loop end value.
	)
{
	m_uLoopEnd = in_uLoopEnd;
	m_uVirtualBufferingSize = ComputeVirtualBuffering();
}

// Helper: compute virtual buffering from scratch.
AkUInt32 CAkAutoStmDeferredLinedUp::ComputeVirtualBuffering()
{
	AkUInt32 uVirtualBuffering = 0;
	AkUInt32 uNumGranted = m_uNextToGrant;
	{
		AkBufferList::Iterator it = m_listBuffers.Begin();
		while ( it != m_listBuffers.End() )
		{
			if ( uNumGranted == 0 )
				break;
			uNumGranted--;
			++it;
		}
		while ( it != m_listBuffers.End() )
		{
			uVirtualBuffering += GetEffectiveViewSize( (*it) );
			++it;
		}
	}
	{
		AkStmMemViewList::Iterator it = m_listPendingXfers.Begin();
		while ( it != m_listPendingXfers.End() )
		{
			uVirtualBuffering += GetEffectiveViewSize( (*it) );
			++it;
		}
	}

	return uVirtualBuffering;
}

#ifdef _DEBUG
void CAkAutoStmDeferredLinedUp::CheckVirtualBufferingConsistency()
{
	AKASSERT( ComputeVirtualBuffering() == m_uVirtualBufferingSize );
	
	// Also check all transfers marked as cancelled but not removed from task yet.
	AkStmMemViewListLight::Iterator it = m_listCancelledXfers.Begin();
	while ( it != m_listCancelledXfers.End() )
	{
		// When cancelled, they should have been cleared.
		AKASSERT( (*it)->Size() == 0 );
		++it;
	}
}
#endif
