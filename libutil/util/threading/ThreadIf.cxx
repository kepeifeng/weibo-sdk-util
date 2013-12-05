#include "stdafx.h"

#if defined(_WIN32)
#include <iostream>
#include <tchar.h>
#include <ctime>

#ifdef _WIN32_WCE
typedef LPTHREAD_START_ROUTINE THREAD_START_ROUTINE;
#else
#include <process.h> // for _beginthreadex()
typedef unsigned(__stdcall *THREAD_START_ROUTINE)(void*);
#endif

#endif

#include <cassert>
#include <iostream>
#include <util/threading/ThreadIf.hxx>
#include <util/threading/Mutex.hxx>
#include <util/threading/Lock.hxx>

using namespace Util;

extern "C"
{
   void*
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
#ifdef _WIN32_WCE
      WINAPI
#else
      __stdcall
#endif
#else

#endif
      ThreadIf::threadWrapper( void* threadParm )
   {
      assert( threadParm );
      ThreadIf* threadObj = static_cast < ThreadIf* > ( threadParm );

      assert( threadObj );
      //#if defined(_WIN32)
      //   srand(unsigned(time(0)) ^ unsigned(GetCurrentThreadId()) ^ unsigned(GetCurrentProcessId()));
      //#endif
      threadObj->thread();
      if (threadObj->isSelfDestroy())
      {
         threadObj->OnBeforeSelfDestroy(threadObj);
         delete threadObj;
      }
      else
      {
         threadObj->mId = 0;
      }
#if defined(_WIN32)
# ifdef _WIN32_WCE
      ExitThread( 0 );
# else
      _endthreadex(0);
# endif
#endif
      return 0;
   }
}

ThreadIf::ThreadIf(bool selfDestroy) : 
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
mThread(0),
#endif
mId(0),
mName(NULL),
mShutdown(false),
mSelfDestroy(selfDestroy)
{
#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
   RESET_PTHREAD_T(mThread);
#endif
}


ThreadIf::~ThreadIf()
{
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
   if (mThread)
   {
      //DWORD exitCode;
      //if (GetExitCodeThread(mThread,&exitCode) != 0)
      //{
      //   assert(exitCode != STILL_ACTIVE && "About to close an active thread handle");
      //}
      CloseHandle(mThread);
   }
#endif
}

void
ThreadIf::run()
{
#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
   assert(mId == NULL);
   if (mId != NULL)
   {
      return;
   }
#else
   assert(mId == 0);
   if (mId != 0)
      return;
#endif

#if defined(_WIN32) 
# if !defined(HAVE_LIBPTHREAD)
   //#if defined(WIN32)
   // !kh!
   // Why _beginthreadex() instead of CreateThread():
   //   http://support.microsoft.com/support/kb/articles/Q104/6/41.ASP
   // Example of using _beginthreadex() mixed with WaitForSingleObject() and CloseHandle():
   //   http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclib/html/_crt__beginthread.2c_._beginthreadex.asp

   mThread =
#ifdef _WIN32_WCE
      // there is no _beginthreadex() for WINCE
      CreateThread(
#else
      (HANDLE)_beginthreadex(
#endif // _WIN32_WCE

      NULL, // LPSECURITY_ATTRIBUTES lpThreadAttributes,  // pointer to security attributes
      0, // DWORD dwStackSize,                         // initial thread stack size
      THREAD_START_ROUTINE
      (ThreadIf::threadWrapper), // LPTHREAD_START_ROUTINE lpStartAddress,     // pointer to thread function
      this, //LPVOID lpParameter,                        // argument for new thread
      0, //DWORD dwCreationFlags,                     // creation flags
      (unsigned *)&mId// LPDWORD lpThreadId                         // pointer to receive thread ID
      );
   assert( mThread != 0 );
# else
   // spawn the thread
   if ( int retval = pthread_create( &mThread, 0, ThreadIf::threadWrapper, this) )
   {
      std::cerr << "Failed to spawn thread: " << retval << std::endl;
      assert(0);
      // TODO - ADD LOGING HERE
   }
   mId = mThread.p;
# endif
#else
   // spawn the thread
   if ( int retval = pthread_create( &mThread, 0, ThreadIf::threadWrapper, this) )
   {
      std::cerr << "Failed to spawn thread: " << retval << std::endl;
      assert(0);
      // TODO - ADD LOGING HERE
   }
   mId = reinterpret_cast<Id>(mThread);
#endif
}

void
ThreadIf::join()
{
#if defined(_WIN32)
# if !defined(HAVE_LIBPTHREAD)
   if (mId == NULL)
   {
      return;
   }

   DWORD exitCode;
   while (true)
   {
      if (GetExitCodeThread(mThread,&exitCode) != 0)
      {
         if (exitCode != STILL_ACTIVE)
         {
            break;
         }
         else
         {
            WaitForSingleObject(mThread,INFINITE);
         }
      }
      else
      {
         // log something here
         break;
      }
   }

   if (mThread)
   {
      CloseHandle(mThread);
      mThread = 0;
   }
# else
   void* stat = NULL;
   if (mThread.p != pthread_self().p)
   {
      int r = pthread_join( mThread , &stat );
      if ( r != 0 )
      {
         //InfoLog( << "pthread_join() returned " << r );
         assert(0);
         // TODO
      }
   }
   RESET_PTHREAD_T(mThread);
# endif
#else
   void* stat = NULL;
   if ( mId != reinterpret_cast<Id>(pthread_self()))
   {
      int r = pthread_join( mThread , &stat );
      if ( r != 0 )
      {
         //InfoLog( << "pthread_join() returned " << r );
         assert(0);
         // TODO
      }
   }
   RESET_PTHREAD_T(mThread);
#endif
   mId = 0;
}

#if defined(_WIN32) && defined (HAVE_LIBPTHREAD)
ThreadIf::Id
ThreadIf::selfId()
{
   return pthread_self().p;
}
#elif !defined(_WIN32) || defined (HAVE_LIBPTHREAD)
ThreadIf::Id
ThreadIf::selfId()
{
   return reinterpret_cast<Id>(pthread_self());
}
#endif

void
ThreadIf::shutdown()
{
#if 0
   WLock lock(mShutdownMutex);
#else
   Lock lock(mShutdownMutex);
#endif
   if (!mShutdown)
   {
      mShutdown = true;
      //mShutdownCondition.signal();
   }
}

//bool
//ThreadIf::waitForShutdown(int ms) const
//{
//   Lock lock(mShutdownMutex);
//   mShutdownCondition.wait(mShutdownMutex, ms);
//   return mShutdown;
//}

bool
ThreadIf::isShutdown() const
{
#if 0
   RLock lock(mShutdownMutex);
#else
   Lock lock(mShutdownMutex);
#endif

   return ( mShutdown );
}

bool
ThreadIf::isRunning() const
{
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
   //#if defined(WIN32)
   DWORD exitCode;
   return (mThread && GetExitCodeThread(mThread,&exitCode) != 0 && exitCode == STILL_ACTIVE);
#else
   int policy;
   sched_param param;
   return (mId != 0 && pthread_getschedparam(mThread, &policy, &param) == 0);
#endif
}

// static
void ThreadIf::sleep(unsigned int ms)
{
#if defined(WIN32)
   ::Sleep(ms);
#else
   ::usleep(ms * 1000);
#endif
}
