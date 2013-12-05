#ifndef UTIL_THREADIF_HXX
#define UTIL_THREADIF_HXX

#include <util/util.h>

#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
//#ifdef _WIN32
#include <BaseTsd.h>
//#  include <winbase.h>
#else
#  include <pthread.h>
#endif

#include <util/threading/Mutex.hxx>
#include <util/threading/Condition.hxx>

#include <util/delegate/delegate.hxx>


#if defined(_WIN32) && defined(HAVE_LIBPTHREAD)
#define RESET_PTHREAD_T(threadId) \
   do { memset(&threadId, 0, sizeof(pthread_t)); }while(0)

#define ASSIGN_PTHREAED_ID(threadId, oldThreadId) \
   do { threadId.p = oldThreadId.p; }while(0)
#else
#define RESET_PTHREAD_T(threadId) \
   do { memset(&threadId, 0, sizeof(pthread_t)); }while(0)

#define ASSIGN_PTHREAED_ID(threadId, oldThreadId) \
   do { threadId = oldThreadId; }while(0)
#endif

namespace Util
{

   /* A wrapper class to create and spawn a thread.  It is a base class.
   ThreadIf::thread() is a pure virtual method .

   <P>Usage:
   To use this class, derive from it and override the thread() method.
   To start the thread, call the run() method.  The code in thread() will
   run in a separate thread.

   <P>Call shutdown() from the constructing thread to shut down the
   code.  This will set the bool shutdown_ to true.  The code in
   thread() should react properly to shutdown_ being set, by
   returning.  Call join() to join the code.
   <P>Sample:
   <PRE>
   ...
   DerivedThreadIf thread;
   thread.run();
   ... do stuff ...
   thread.shutdown();
   thread.join();
   </PRE>
   */
   class UTIL_API ThreadIf;

   typedef Util::Delegate<void (ThreadIf*)> ThreadIfNotifyEvent;

   class UTIL_API ThreadIf
   {
   public:
      static void sleep(unsigned int ms);

   public:
      ThreadIf(bool selfDestroy = false);
      virtual ~ThreadIf();

      // runs the code in thread() .  Returns immediately
      virtual void run();

      // joins to the thread running thread()
      void join();

      // request the thread running thread() to return, by setting  mShutdown
      virtual void shutdown();

      //waits for waitMs, or stops waiting and returns true if shutdown was
      //called
      //bool waitForShutdown(int ms) const;

      // returns true if the thread has been asked to shutdown or not running
      bool isShutdown() const;
      bool isRunning() const;

      bool isSelfDestroy() const { return mSelfDestroy; }
      void setSelfDestry(bool selfDestroy) { mSelfDestroy = selfDestroy; }

      void setName(const char* name) { mName = name; }

      typedef void* Id;
#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
      static Id selfId();
#endif

   protected:
      /* thread is a virtual method.  Users should derive and define
      thread() such that it returns when isShutdown() is true.
      */
      virtual void thread() = 0;
      
   public: // event
      ThreadIfNotifyEvent OnBeforeSelfDestroy;

   protected:
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
      HANDLE mThread;
#else
      pthread_t mThread;
#endif
      Id mId;
      const char* mName;

      bool mShutdown;
#if 0
      mutable RWMutex mShutdownMutex;
#else
      mutable Mutex mShutdownMutex;
#endif
      //mutable Condition mShutdownCondition;

   private:
      bool mSelfDestroy;

   private:
      // Suppress copying
      ThreadIf(const ThreadIf &);
      const ThreadIf & operator=(const ThreadIf &);

   private:
      static void*
//#ifdef _WIN32
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
#ifdef _WIN32_WCE
         WINAPI
#else
         __stdcall
#endif
#else
   
#endif
         threadWrapper( void* threadParm );

   };

} // namespace Util

#endif // #ifndef UTIL_THREADIF_HXX


