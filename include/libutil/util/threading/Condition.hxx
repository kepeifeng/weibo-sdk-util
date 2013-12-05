#ifndef UTIL_CONDITION_HXX
#define UTIL_CONDITION_HXX

#include <util/util.h>

#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
#  include <windows.h>
#  include <winbase.h>
#else
#  include <pthread.h>
#endif

// !kh!
// Attempt to resolve POSIX behaviour conformance for win32 build.
#if !defined(HAVE_LIBPTHREAD)
#define _CONDITION_WIN32_CONFORMANCE_TO_POSIX
#endif

namespace Util
{
   class UTIL_API Mutex;

   class UTIL_API Condition
   {
   public:
      Condition();
      ~Condition();

      void wait (Mutex& mtx);
      /** returns true if the condition was woken up by activity, false if timeout.
      *  or interrupt.
      */
      bool wait (Mutex& mutex, unsigned int ms);

      // !kh!
      //  deprecate these?
      void wait (Mutex* mutex);
      bool wait (Mutex* mutex, unsigned int ms);

      /** Signal one waiting thread.
      *  Returns 0, if successful, or an errorcode.
      */
      void signal();

      /** Signal all waiting threads.
      *  Returns 0, if successful, or an errorcode.
      */
      void broadcast();

   private:
      // !kh!
      //  no value sematics, therefore private and not implemented.
      Condition (const Condition&);
      Condition& operator= (const Condition&);

   private:
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
      //#ifdef _WIN32
#  ifdef _CONDITION_WIN32_CONFORMANCE_TO_POSIX
      // !kh!
      // boost clone with modification
      // credit boost?
      void enterWait ();
      void* m_gate;
      void* m_queue;
      void* m_mutex;
      unsigned m_gone;  // # threads that timed out and never made it to m_queue
      unsigned long m_blocked; // # threads blocked on the condition
      unsigned m_waiting; // # threads no longer waiting for the condition but
      // still waiting to be removed from m_queue
#  else
      HANDLE mId;
#  endif
#else
      mutable pthread_cond_t mId;
#endif
   };

} // namespace Util

#endif // #ifndef UTIL_CONDITION_HXX

