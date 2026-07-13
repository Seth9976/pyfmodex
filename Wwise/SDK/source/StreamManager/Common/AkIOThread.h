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

#ifndef _AK_IO_THREAD_H_
#define _AK_IO_THREAD_H_

#include <AK/Tools/Common/AkObject.h>
#include <AK/Tools/Common/AkLock.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/SoundEngine/Common/AkAtomic.h>

// ------------------------------------------------------------------------------
// Defines.
// ------------------------------------------------------------------------------

namespace AK
{
	namespace StreamMgr
	{
		//-----------------------------------------------------------------------------
		// Name: class CAkClientThreadAware
		// Desc: Platform-specific layer for stream tasks which handles blocking client
		//		 threads for blocking I/O.
		//		 Client threads are blocked using a conditional variable
		//		 and signaling it for a specific thread (sys_cond_signal_to).
		//-----------------------------------------------------------------------------
		class CAkClientThreadAware
		{
		public:

			// Platform-independent interface.
			// Platform-independent implementation calls these methods.
			//-----------------------------------------------------------------------------
			CAkClientThreadAware();
			virtual ~CAkClientThreadAware();

			void SetBlockedStatus();

			inline bool IsBlocked()
			{
				return m_idBlockedThread != AK_NULL_THREAD;
			}

			void WaitForIoCompletion();
			void SignalIoCompletion();

		private:
			AkEvent	m_evtWaitingForIo;
			AkThreadID	m_idBlockedThread;	// ID of thread that is waiting for I/O to complete.	
		};

		//-----------------------------------------------------------------------------
		// Name: CAkIOThread
		// Desc: Implements the I/O thread, and synchronization services. Namely,
		//		 thread control based on standard and automatic streams semaphores,
		//		 memory full, max number of concurrent low-level transfers. It also 
		//		 works with client thread blocking (partly implemented in 
		//		 CAkClientThreadAware).
		//       The I/O thread calls pure virtual method PerformIO(), that has to be
		//       implemented by derived classes according to how they communicate with
		//       the Low-Level IO.
		//-----------------------------------------------------------------------------
		class CAkIOThread
		{
		public:

			CAkIOThread();
			virtual ~CAkIOThread();

			// Methods used by stream objects.
			// --------------------------------------------------------

			// Blocking I/O.
			void WaitForIOCompletion(
				CAkClientThreadAware* in_pWaitingTask // Stream objects must call this method with this for synchronization purposes:
			);

			void SignalIOCompleted(
				CAkClientThreadAware* in_pWaitingTask	// The caller's thread id must be checked after the mutex was obtained to avoid race conditions.
			);

			// Scheduler thread control.
			// Semaphore usage:
			//StdSem: Signaled whenever at least one standard stream is waiting for IO (pending operation).
			//AutoSem: Signaled whenever at least one automatic stream is waiting for IO (running), and memory
			//usage allows it.
			//Memory notifications: When set to "idle", AutoSem event is inhibited: the I/O thread will not
			//wake up to schedule automatic streams. This occurs when there is no more memory, and the scheduler
			//decides not to reassign buffers.
			//It is signaled whenever a streaming buffer is freed (ReleaseBuffer(), Destroy(), SetPosition()->Flush()),
			//or if a new stream is created (requires memory usage reevaluation), through NotifyMemChange().

			void StdStmInc();      // Increment standard streams count (signal).
			void StdStmDec();      // Decrement standard streams count (reset if count=0).
			void AutoStmInc();     // Increment automatic streams count (signal).
			void AutoStmDec();     // Decrement automatic streams count (reset if count=0).
			
			void NotifyMemChange(); // Notify that a streaming buffer was freed.
			void NotifyMemFull();   // Notify that all streaming buffers are full, stopping new Auto Stms.

			// Query memory status.
			inline bool CannotScheduleAutoStreams() { return m_bAutoStmMemFull; }
			
			// Concurrent I/O requests.
			void IncrementIOCount();
			void DecrementIOCount();
			inline AkInt32 GetNumConcurrentIO() { return AkAtomicLoad32(&m_uNumConcurrentIO); }

			// I/O Algorithm. Depends on the type of device. 
			// Implement in derived class.
			virtual void PerformIO() = 0;

		protected:
			// Init/term scheduler objects.
			AKRESULT Init(const AkThreadProperties& in_threadProperties, bool bUseIOThread);

			void Term();

			// Destroys all streams.
			virtual bool WaitForStreamCleanup() = 0;

			// Called once when I/O thread starts.
			virtual void OnThreadStart() = 0;

			// The I/O thread asks the device if it is ready to schedule through this method.
			// If it is not, wait for the CondVar.
			inline bool CanSchedule()
			{
				return (m_uNumConcurrentIO < m_uMaxConcurrentIO &&
					(m_numPendingStdStms > 0 ||
					(m_numPendingAutoStms > 0 && !m_bAutoStmMemFull)) );
			}

			void ClearThreadData();

			// IO thread management
			static AK_DECLARE_THREAD_ROUTINE(IOSchedThread);
			AkThread    m_hIoThread;
			AkEvent     m_hIoThreadEvent;
			AkThreadProperties m_threadProperties;

			AkAtomic32  m_numPendingStdStms;  // Number of standard stream tasks waiting for I/O. 
			AkAtomic32  m_numPendingAutoStms; // Number of standard auto tasks waiting for I/O. 
			bool        m_bAutoStmMemFull;    // When true, automatic streams semaphore is inhibited, because there are no free buffers, and they cannot be reassigned. 

			AkAtomic32  m_uNumConcurrentIO;   // Number of concurrent requests sent to LowLevelIO.

		protected:
			AkInt32     m_uMaxConcurrentIO;
			bool        m_bThreadActive;

		};
	}
}
#endif //_AK_DEVICE_BASE_H_
