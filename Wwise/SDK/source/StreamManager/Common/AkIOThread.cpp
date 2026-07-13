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
// AkIOThread.h
//
// Platform-specific layer of high-level I/O devices of the Stream Manager.
// Implements the I/O thread function, manages all thread related 
// synchronization.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AkIOThread.h"
#include <AK/Tools/Common/AkInstrument.h>


using namespace AK::StreamMgr;

//-----------------------------------------------------------------------------
// Name: class CAkClientThreadAware
// Desc: Platform-specific layer for stream tasks which handles blocking client
//		 threads for blocking I/O.
//		 Client threads are blocked using a conditional variable
//		 and signaling it for a specific thread (sys_cond_signal_to).
//-----------------------------------------------------------------------------

CAkClientThreadAware::CAkClientThreadAware()
: m_idBlockedThread( AK_NULL_THREAD )
{
	AKPLATFORM::AkCreateEvent(m_evtWaitingForIo);
}

CAkClientThreadAware::~CAkClientThreadAware()
{
	AKASSERT( !IsBlocked() );
	AKPLATFORM::AkDestroyEvent(m_evtWaitingForIo);
}

// Set task as "blocked, waiting for I/O completion".
void CAkClientThreadAware::SetBlockedStatus() 
{ 
	AKASSERT(!IsBlocked());
	// Having a non-zero m_idBlockedThread indicates that this stream is waiting for a signal.
	m_idBlockedThread = AKPLATFORM::CurrentThread();
}

void CAkClientThreadAware::WaitForIoCompletion()
{
	while (IsBlocked())
	{
		AKPLATFORM::AkWaitForEvent(m_evtWaitingForIo);
	}
}

void CAkClientThreadAware::SignalIoCompletion()
{
	m_idBlockedThread = AK_NULL_THREAD;
	AKPLATFORM::AkSignalEvent(m_evtWaitingForIo);
}

//-----------------------------------------------------------------------------
// Name: CAkIOThread
// Desc: Implements the I/O thread, and synchronization services. Namely,
// thread control based on standard and automatic streams semaphores,
// memory full, max number of concurrent low-level transfers. It also 
// works with client thread blocking (partly implemented in 
// CAkClientThreadAware).
// The I/O thread calls pure virtual method PerformIO(), that has to be
// implemented by derived classes according to how they communicate with
// the Low-Level IO.
//-----------------------------------------------------------------------------

// Device construction/destruction.
CAkIOThread::CAkIOThread()
: m_numPendingStdStms( 0 )
, m_numPendingAutoStms(0)
, m_bAutoStmMemFull(false)
, m_uNumConcurrentIO( 0 )

{	
	ClearThreadData();
}

CAkIOThread::~CAkIOThread()
{
	AKPLATFORM::AkDestroyEvent(m_hIoThreadEvent);
	ClearThreadData();
}

void CAkIOThread::ClearThreadData()
{
	AKPLATFORM::AkClearThread(&m_hIoThread);
	AKPLATFORM::AkClearEvent(m_hIoThreadEvent);
}

// Init/term scheduler objects.
AKRESULT CAkIOThread::Init(const AkThreadProperties& in_threadProperties, bool bUseIOThread)
{
	AkAtomicStore32(&m_numPendingStdStms, 0);
	AkAtomicStore32(&m_numPendingAutoStms, 0);
	m_uNumConcurrentIO	= 0;
	
	if (AKPLATFORM::AkCreateEvent(m_hIoThreadEvent) != AK_Success)
		return AK_Fail;

#if defined(AK_SUPPORT_THREADS)
	if (bUseIOThread)
	{
		// Launch the scheduler/IO thread.
		// Create and start the worker IO thread with default stack size.

		m_threadProperties = in_threadProperties;
		m_bThreadActive = true;
		AKPLATFORM::AkCreateThread(
			CAkIOThread::IOSchedThread,			// Start address
			this,								// Parameter
			in_threadProperties,				// Properties
			&m_hIoThread,						// Handle
			"AK::IOThread",
			nullptr);

		if (AKPLATFORM::AkIsValidThread(&m_hIoThread))
			return AK_Success;

		return AK_Fail;
	}	
#endif
	return AK_Success;
}

void CAkIOThread::Term()
{
	m_bThreadActive = false;
		
#if defined(AK_SUPPORT_THREADS)	
	if ( AKPLATFORM::AkIsValidThread( &m_hIoThread ) )
	{
		AKPLATFORM::AkSignalEvent(m_hIoThreadEvent);

		AKPLATFORM::AkWaitForSingleThread( &m_hIoThread );
		AKPLATFORM::AkCloseThread(&m_hIoThread, nullptr);
	}
#endif
}

#if defined AK_WIN || defined AK_XBOX
//All versions of Windows and XBox need an _alertable_ wait
#define WAIT_FOR_IO_EVENT(_evt_) ::WaitForSingleObjectEx( _evt_, INFINITE, TRUE )
#else
#define WAIT_FOR_IO_EVENT(_evt_) AKPLATFORM::AkWaitForEvent(_evt_)
#endif

#if defined(AK_SUPPORT_THREADS)
// --------------------------------------------------------
// Scheduler thread.
AK_DECLARE_THREAD_ROUTINE( CAkIOThread::IOSchedThread )
{	
	CAkIOThread * pDevice = AK_GET_THREAD_ROUTINE_PARAMETER_PTR( CAkIOThread );
	AK_THREAD_INIT_CODE(pDevice->m_threadProperties);

	AK_INSTRUMENT_THREAD_START( "CAkIOThread::IOSchedThread" );
	AK::MemoryMgr::InitForThread();
	
	pDevice->OnThreadStart();
	
	do
	{
		// The I/O thread needs to wait before being woken up
		WAIT_FOR_IO_EVENT(pDevice->m_hIoThreadEvent);
		pDevice->PerformIO();
	} while (pDevice->m_bThreadActive);

	AK::MemoryMgr::TermForThread();
	AkExitThread(AK_RETURN_THREAD_OK);
}
#endif

// --------------------------------------------------------
// Increment pending standard streams count.
void CAkIOThread::StdStmInc()
{
	AkAtomicInc32(&m_numPendingStdStms);
	AKPLATFORM::AkSignalEvent(m_hIoThreadEvent);
}

// Decrement pending standard streams count.
void CAkIOThread::StdStmDec()
{
	AkAtomicDec32(&m_numPendingStdStms);
}

// Increment pending automatic streams count.
// Note: Event remains unsignaled if there is no memory ("Mem Idle" state).
// Sync: Streams semaphore lock (mutex). Signals CondVar if I/O thread was idle.
void CAkIOThread::AutoStmInc()
{
	AkAtomicInc32(&m_numPendingAutoStms);
	AKPLATFORM::AkSignalEvent(m_hIoThreadEvent);
}

// Decrement pending automatic streams count.
// Sync: Automatic streams semaphore lock.
void CAkIOThread::AutoStmDec()
{
	AkAtomicDec32(&m_numPendingAutoStms);
}

// Notify that memory was freed, or memory usage must be reviewed.
// Un-inhibits automatic streams semaphore. 
void CAkIOThread::NotifyMemChange()
{
	m_bAutoStmMemFull = false;
	AKPLATFORM::AkSignalEvent(m_hIoThreadEvent);
}

// Notify that memory is full. I/O thread should not wake up to service automatic streams until memory usage
// changes (until someone calls NotifyMemChange). Inhibits automatic streams.
void CAkIOThread::NotifyMemFull()
{
	m_bAutoStmMemFull = true;
}

// --------------------------------------------------------
// Concurrent I/O requests.
void CAkIOThread::IncrementIOCount()
{
	AkAtomicInc32(&m_uNumConcurrentIO);
}

void CAkIOThread::DecrementIOCount()
{
	AkAtomicDec32(&m_uNumConcurrentIO);
	AKPLATFORM::AkSignalEvent(m_hIoThreadEvent);
}

// --------------------------------------------------------
// Blocking I/O.
void CAkIOThread::WaitForIOCompletion(CAkClientThreadAware* in_pWaitingTask)
{
#if defined(AK_SUPPORT_THREADS)
	// Signal the ioThreadEvent so that we can consume the work we're waiting on
	AKPLATFORM::AkSignalEvent(m_hIoThreadEvent);
	in_pWaitingTask->WaitForIoCompletion();
#else
	// In a single-threaded system, process I/O right here until the task is complete.
	while (in_pWaitingTask->IsBlocked())
		PerformIO();
#endif
}

void CAkIOThread::SignalIOCompleted(CAkClientThreadAware* in_pWaitingTask)
{
	in_pWaitingTask->SignalIoCompletion();
}
