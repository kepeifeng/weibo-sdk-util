#include "stdafx.h"

#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
#  include <pthread.h>
#  include <errno.h>
#endif

#include <stdexcept>
using namespace std;

#include <boost/config.hpp>

#include <cassert>
#include <climits>

#include <util/common/TimeUtil.hxx>
#include <util/threading/Condition.hxx>
#include <util/threading/Mutex.hxx>

using namespace Util;

Condition::Condition()
#if defined(_WIN32) && defined(HAVE_LIBPTHREAD)
:mId(PTHREAD_COND_INITIALIZER)
#endif
{
   //cerr << this << " Condition::Condition" << endl;
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
//#ifdef _WIN32
#  ifdef _CONDITION_WIN32_CONFORMANCE_TO_POSIX
   m_blocked = 0;
   m_gone = 0;
   m_waiting = 0;
   m_gate = reinterpret_cast<void*>(CreateSemaphore(0, 1, 1, 0));
   if (!m_gate)
   {
      assert(0);
#ifndef BOOST_NO_EXCEPTIONS
      throw runtime_error("CreateSemaphore API failed");
#else
      return;
#endif
   }
   m_queue = reinterpret_cast<void*>(
      //        CreateSemaphore(0, 0, (numeric_limits<long>::max)(), 0));
      CreateSemaphore(0, 0, LONG_MAX, 0));
   if (!m_queue)
   {
      assert(0);
      CloseHandle(reinterpret_cast<HANDLE>(m_gate));
#ifndef BOOST_NO_EXCEPTIONS
      throw runtime_error("CreateSemaphore API failed");
#else
      return;
#endif
   }
   m_mutex = reinterpret_cast<void*>(CreateMutex(0, 0, 0));
   if (!m_mutex)
   {
      assert(0);
      CloseHandle(reinterpret_cast<HANDLE>(m_gate));
      CloseHandle(reinterpret_cast<HANDLE>(m_queue));
#ifndef BOOST_NO_EXCEPTIONS
      throw runtime_error("CreateMutex API failed");
#else
      return NULL;
#endif
   }
#  else
   mId =  CreateEvent(
      NULL, //LPSECURITY_ATTRIBUTES lpEventAttributes,
      // pointer to security attributes
      FALSE, // BOOL bManualReset,  // flag for manual-reset event
      FALSE, //BOOL bInitialState, // flag for initial state
      NULL //LPCTSTR lpName      // pointer to event-object name
      );
   if (!mId)
   {
      assert(0);
#ifndef BOOST_NO_EXCEPTIONS
      throw runtime_error("CreateEvent API failed");
#else
      return;
#endif
   }
#  endif
#else
   int  rc =  pthread_cond_init(&mId,0);
   if (rc != 0)
   {
      assert(0);
#ifndef BOOST_NO_EXCEPTIONS
      throw runtime_error("pthread_cond_init API failed");
#else
      return;
#endif
   }
#endif
}


Condition::~Condition ()
{
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
//#ifdef _WIN32
#  ifdef _CONDITION_WIN32_CONFORMANCE_TO_POSIX
   int res = 0;
   res = CloseHandle(reinterpret_cast<HANDLE>(m_gate));
   assert(res);
   res = CloseHandle(reinterpret_cast<HANDLE>(m_queue));
   assert(res);
   res = CloseHandle(reinterpret_cast<HANDLE>(m_mutex));
   assert(res);
#  else
   BOOL ok = CloseHandle(mId);
   assert( ok );
#  endif
#else
   if (pthread_cond_destroy(&mId) == EBUSY)
   {
      //WarningLog (<< "Condition variable is busy");
      assert(0);
   }
#endif
}

#if defined(_WIN32) && defined(_CONDITION_WIN32_CONFORMANCE_TO_POSIX)
void
Condition::enterWait ()
{
   int res = 0;
   res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
   assert(res == WAIT_OBJECT_0);
   ++m_blocked;
   res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
   assert(res);
}
#endif

void
Condition::wait (Mutex& mutex)
{
   //cerr << "Condition::wait " << mutex << endl;
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
//#ifdef _WIN32
#   ifdef _CONDITION_WIN32_CONFORMANCE_TO_POSIX
   this->enterWait();

   mutex.unlock();
   // do wait
   {
      int res = 0;
      res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_queue), INFINITE);
      assert(res == WAIT_OBJECT_0);

      unsigned was_waiting=0;
      unsigned was_gone=0;

      res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
      assert(res == WAIT_OBJECT_0);
      was_waiting = m_waiting;
      was_gone = m_gone;
      if (was_waiting != 0)
      {
         if (--m_waiting == 0)
         {
            if (m_blocked != 0)
            {
               res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1,
                  0); // open m_gate
               assert(res);
               was_waiting = 0;
            }
            else if (m_gone != 0)
               m_gone = 0;
         }
      }
      //else if (++m_gone == ((numeric_limits<unsigned>::max)() / 2))
      else if (++m_gone == (ULONG_MAX / 2))
      {
         // timeout occured, normalize the m_gone count
         // this may occur if many calls to wait with a timeout are made and
         // no call to notify_* is made
         res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
         assert(res == WAIT_OBJECT_0);
         m_blocked -= m_gone;
         res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
         assert(res);
         m_gone = 0;
      }
      res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
      assert(res);

      if (was_waiting == 1)
      {
         for (/**/ ; was_gone; --was_gone)
         {
            // better now than spurious later
            res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_queue),
               INFINITE);
            assert(res == WAIT_OBJECT_0);
         }
         res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
         assert(res);
      }
   }

   mutex.lock();
#   else
   // FixMe: Race condition between time we get mId and when we
   // re-acquire the mutex.
   mutex.unlock();
   WaitForSingleObject(mId,INFINITE);
   mutex.lock();
#   endif
#else
   int rc = pthread_cond_wait(&mId, mutex.getId());
   (void)rc;
   assert( rc == 0 );
#endif
}
void
Condition::wait (Mutex* mutex)
{
   this->wait(*mutex);
}

bool
Condition::wait(Mutex& mutex, 
                unsigned int ms)
{
   if (ms == 0)
   {
      wait(mutex);
      return true;
   }

#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
//#ifdef _WIN32
#   ifdef _CONDITION_WIN32_CONFORMANCE_TO_POSIX
   this->enterWait();

   mutex.unlock();

   //  do timed wait
   bool ret = false;
   {
      unsigned int res = 0;
      UInt64  start = TimeUtil::getTimeMs();

      for (;;)
      {
         res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_queue),
            ms);
         assert(res != WAIT_FAILED && res != WAIT_ABANDONED);
         ret = (res == WAIT_OBJECT_0);
         if (res == WAIT_TIMEOUT)
         {
            UInt64  now = TimeUtil::getTimeMs();
            unsigned int elapsed = (unsigned int)(now - start);
            if (ms > elapsed)
            {
               ms -= elapsed;
               continue;
            }
         }

         break;
      }

      unsigned was_waiting=0;
      unsigned was_gone=0;

      res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
      assert(res == WAIT_OBJECT_0);
      was_waiting = m_waiting;
      was_gone = m_gone;
      if (was_waiting != 0)
      {
         if (!ret) // timeout
         {
            if (m_blocked != 0)
               --m_blocked;
            else
               ++m_gone; // count spurious wakeups
         }
         if (--m_waiting == 0)
         {
            if (m_blocked != 0)
            {
               res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1,
                  0); // open m_gate
               assert(res);
               was_waiting = 0;
            }
            else if (m_gone != 0)
               m_gone = 0;
         }
      }
      else if (++m_gone == (ULONG_MAX / 2))
      {
         // timeout occured, normalize the m_gone count
         // this may occur if many calls to wait with a timeout are made and
         // no call to notify_* is made
         res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
         assert(res == WAIT_OBJECT_0);
         m_blocked -= m_gone;
         res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
         assert(res);
         m_gone = 0;
      }
      res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
      assert(res);

      if (was_waiting == 1)
      {
         for (/**/ ; was_gone; --was_gone)
         {
            // better now than spurious later
            res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_queue),
               INFINITE);
            assert(res ==  WAIT_OBJECT_0);
         }
         res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
         assert(res);
      }

   }

   mutex.lock();
   return ret;

#   else
   // FixMe: Race condition between time we get mId and when we
   // re-acquire the mutex.
   //
   // SLG: A Note about the Win32 Implementation of Conditions
   //
   // I have investigated a fix for this.  A solution to this problem is
   // non-trivial.  Please read http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
   // for a full explanation.  This is an implementation of the SetEvent solution
   // discussed in that article.  This solution has the following issues:
   // 1.  Unfairness - ie.  First thread to call wait may not be first thread
   //     to be released from condition.
   // 2.  Incorrectness due to a race condition when a broadcast occurs
   // (see the link for more details on these issues)
   //
   // There is a solution that corrects these two problem, but also introduces 2 more.
   // This solution (also discussed in the link) requires the use of a primitive only
   // available in WinNT and above.  It also requires that the Mutex passed in be
   // implemented using windows Mutexes instead of CriticalSections - they are less
   // efficient.  Thus the problems with this SignalObjectAndWait solution are:
   // 1.  Not portable to all versions of windows - ie.  will not work with Win98/Me
   // 2.  Less efficient than tthe SetEvent solution
   //
   // I have choosen to stick with the SetEvent Solution for the following reasons:
   // 1.  Speed is important.
   // 2.  The Unfairness issue is not really a big problem since the stack currently
   //     does not call a wait function from two different threads.  (assuming the
   //     hosting application always calls process() from the same thread).  The only
   //     time multi-threading comes into the picture is when the transports queue
   //     messages from the wire onto the stateMacFifo - but they are retrieved off the
   //     Fifo by a single thread.
   // 3.  The Incorrectness issue is also not a big problem, since the stack currently
   //     doesn't use the broadcast member of this class.
   //
   // Note:  The implementation of broadcast remains incomplete - since it is currently
   //        unused and would require an additional CriticalSection Enter and Leave to
   //        keep track of a counter (see the above link for more info).  This can be
   //        easily added in the future if required.
   mutex.unlock();
   DWORD ret = WaitForSingleObject(mId, ms);
   mutex.lock();
   assert(ret != WAIT_FAILED);
   return (ret == WAIT_OBJECT_0);
#   endif
#else

   struct timeval nowTmp;
   TimeUtil::getTimeOfDay(nowTmp);

   timespec now = {nowTmp.tv_sec + (ms/1000), (nowTmp.tv_usec+((ms % 1000) * 1000))*1000};
   if (now.tv_nsec > 1000000000)
   {
      now.tv_nsec -= 1000000000;
      ++(now.tv_sec);
   }

   //cerr << "Condition::wait " << mutex << "ms=" << ms << " expire=" << expiresTS.tv_sec << " " << expiresTS.tv_nsec << endl;
   int rc = pthread_cond_timedwait(&mId, mutex.getId(), &now);
   if (rc == EINTR || rc == ETIMEDOUT)
   {
      return false;
   }
   else
   {
      //cerr << this << " pthread_cond_timedwait failed " << ret << " mutex=" << mutex << endl;
      assert( rc == 0 );
      return true;
   }
#endif
}
bool
Condition::wait (Mutex* mutex, unsigned int ms)
{
   return this->wait(*mutex, ms);
}

void
Condition::signal ()
{
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
//#ifdef _WIN32
#  ifdef _CONDITION_WIN32_CONFORMANCE_TO_POSIX
   unsigned signals = 0;

   int res = 0;
   res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
   assert(res == WAIT_OBJECT_0);

   if (m_waiting != 0) // the m_gate is already closed
   {
      if (m_blocked == 0)
      {
         res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
         assert(res);
         return;
      }

      ++m_waiting;
      --m_blocked;
      signals = 1;
   }
   else
   {
      res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
      assert(res == WAIT_OBJECT_0);
      if (m_blocked > m_gone)
      {
         if (m_gone != 0)
         {
            m_blocked -= m_gone;
            m_gone = 0;
         }
         signals = m_waiting = 1;
         --m_blocked;
      }
      else
      {
         res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
         assert(res);
      }
   }

   res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
   assert(res);

   if (signals)
   {
      res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_queue), signals, 0);
      assert(res);
   }
#  else
   BOOL ret = SetEvent(
      mId // HANDLE hEvent   // handle to event object
      );
   assert(ret);
#  endif
#else
   int rc = pthread_cond_signal(&mId);
   (void)rc;
   assert( rc == 0);
#endif
}


void
Condition::broadcast()
{
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
//#ifdef _WIN32
#  ifdef _CONDITION_WIN32_CONFORMANCE_TO_POSIX
   unsigned signals = 0;

   int res = 0;
   res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
   assert(res == WAIT_OBJECT_0);

   if (m_waiting != 0) // the m_gate is already closed
   {
      if (m_blocked == 0)
      {
         res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
         assert(res);
         return;
      }

      m_waiting += (signals = m_blocked);
      m_blocked = 0;
   }
   else
   {
      res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
      assert(res == WAIT_OBJECT_0);
      if (m_blocked > m_gone)
      {
         if (m_gone != 0)
         {
            m_blocked -= m_gone;
            m_gone = 0;
         }
         signals = m_waiting = m_blocked;
         m_blocked = 0;
      }
      else
      {
         res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
         assert(res);
      }
   }

   res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
   assert(res);

   if (signals)
   {
      res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_queue), signals, 0);
      assert(res);
   }
#  else
   //assert(0);
#  endif
#else
   pthread_cond_broadcast(&mId);
#endif
}


