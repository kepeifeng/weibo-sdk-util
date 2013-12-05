#ifndef TUTIL_MUTEX_HXX
#define TUTIL_MUTEX_HXX

#include <util/util.h>

#include <util/compat.hxx>

#if defined(_WIN32) && defined(HAVE_LIBPTHREAD)
#  include <pthread.h>
#endif

namespace Util
{
   class UTIL_API Condition;

   class UTIL_API Mutex
   {
      friend class Condition;

   public:
      enum Mode
      {
         Normal = 0,
         Recursive,
      };

   public:
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
      Mutex();
#else
      Mutex(Mode mode = Normal);
#endif

      ~Mutex();
      void lock();
      void unlock();

#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
      bool tryLock();

      Mode mode() const
      {
         return mMode;
      }
#endif

   private:
      // !kh!
      //  no value sematics, therefore private and not implemented.
      Mutex (const Mutex&);
      Mutex& operator= (const Mutex&);

   private:
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
      CRITICAL_SECTION mId;
#else
      Mode mMode;
      mutable  pthread_mutex_t mId;

      const pthread_mutex_t* getId() const;
      pthread_mutex_t* getId();
#endif
   };

} // namespace Util

#endif // #ifndef UTIL_MUTEX_HXX



