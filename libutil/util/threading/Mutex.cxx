#include "stdafx.h"

/*
namespace std
{
typedef wchar_t wint_t;
typedef unsigned int size_t;
}

*/
#include <cassert>
# ifndef _WIN32_WCE
#  include <cerrno>
# endif
#include <util/threading/Mutex.hxx>

#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
#  include <windows.h>
#  include <winbase.h>
#else
#  include <pthread.h>
#endif

using namespace Util;
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
Mutex::Mutex()
#else
Mutex::Mutex(Mode mode)
#endif
#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
:mMode(mode)
# if defined(_WIN32)
,mId(PTHREAD_MUTEX_INITIALIZER)
# endif
#endif
{
#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
   pthread_mutexattr_t attr;
   pthread_mutexattr_init(&attr);
   if (mMode == Mutex::Recursive)
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
   else
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
   int rc = pthread_mutex_init(&mId, &attr);
   (void)rc;
   assert( rc == 0 );
#else
   // Note:  Windows Critical sections are recursive in nature and perhaps
   //        this implementation calls for a non-recursive implementation
   //        (since there also exists a RecursiveMutex class).  The effort
   //        to make this non-recursive just doesn't make sense though. (SLG)
   InitializeCriticalSection(&mId);
#endif
}


Mutex::~Mutex ()
{
#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
   int  rc = pthread_mutex_destroy(&mId);
   (void)rc;
   assert( rc != EBUSY );  // currently locked 
   assert( rc == 0 );
#else
   DeleteCriticalSection(&mId);
#endif
}


void
Mutex::lock()
{
#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
   int  rc = pthread_mutex_lock(&mId);
   (void)rc;
   assert( rc != EINVAL );
   assert( rc != EDEADLK );
   assert( rc == 0 );
#else
   EnterCriticalSection(&mId);
#endif
}

void
Mutex::unlock()
{
#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
   int  rc = pthread_mutex_unlock(&mId);
   (void)rc;
   assert( rc != EINVAL );
   assert( rc != EPERM );
   assert( rc == 0 );
#else
   LeaveCriticalSection(&mId);
#endif
}

#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
bool 
Mutex::tryLock()
{
   int rc = pthread_mutex_trylock(&mId);
   if (rc == EBUSY)
      return false;
   if (rc == 0)
      return true;
   else
      return false;
}

const pthread_mutex_t*
Mutex::getId() const
{
   return &mId;
}

pthread_mutex_t*
Mutex::getId()
{
   return &mId;
}
#endif

