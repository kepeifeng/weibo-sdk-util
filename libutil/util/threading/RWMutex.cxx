#include "stdafx.h"

#include <util/threading/RWMutex.hxx>

#include <cassert>

#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
# include <util/threading/Lock.hxx>
#endif

using namespace Util;

RWMutex::RWMutex()
#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
:mReaderCount(0)
,mWriterHasLock(false)
,mPendingWriterCount(0)
#endif
{
#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
   int ret = pthread_rwlock_init(&mRWLock, NULL);
   assert( ret == 0 );
#endif
}


RWMutex::~RWMutex()
{
#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
   int rc = pthread_rwlock_destroy(&mRWLock);
   (void)rc;
   assert( rc != EBUSY );  // currently locked 
   assert( rc == 0 );
#endif
}


void
RWMutex::readlock()
{
#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
   int rc = pthread_rwlock_rdlock(&mRWLock);
   (void)rc;
   assert( rc != EINVAL );
   assert( rc == 0 );
#else
   Lock theLock(mMutex);

   while ( mWriterHasLock || mPendingWriterCount > 0 )
   {
      mReadCondition.wait(mMutex);
   }

#endif
}


void
RWMutex::writelock()
{
#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
   int rc = pthread_rwlock_wrlock(&mRWLock);
   (void)rc;
   assert( rc != EINVAL );
   assert( rc != EDEADLK );
   assert( rc == 0 );
#else
   Lock theLock(mMutex);

   ++mPendingWriterCount;

   while ( mWriterHasLock || mReaderCount > 0 )
   {
      mPendingWriteCondition.wait(mMutex);
   }

   --mPendingWriterCount;

   mWriterHasLock = true;
#endif
}


void
RWMutex::lock()
{
   writelock();
}


void
RWMutex::unlock()
{
#if !defined(_WIN32) || defined(HAVE_LIBPTHREAD)
   int rc = pthread_rwlock_unlock(&mRWLock);
   (void)rc;
   assert( rc != EINVAL );
   assert( rc != EPERM );
   assert( rc == 0 );
#else
   Lock theLock(mMutex);

   // Unlocking a write lock.
   //
   if ( mWriterHasLock )
   {
      assert( mReaderCount == 0 );

      mWriterHasLock = false;

      // Pending writers have priority. Could potentially starve readers.
      //
      if ( mPendingWriterCount > 0 )
      {
         mPendingWriteCondition.signal();
      }

      // No writer, no pending writers, so all the readers can go.
      //
      else
      {
         mReadCondition.broadcast();
      }

   }

   // Unlocking a read lock.
   //
   else if (mReaderCount)
   {
      //assert( mReaderCount > 0 );

      --mReaderCount;

      if ( mReaderCount == 0 && mPendingWriterCount > 0 )
      {
         mPendingWriteCondition.signal();
      }
   }
   else
   {
      assert(false);
   }
#endif
}

#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
unsigned int
RWMutex::readerCount() const
{
   return ( mReaderCount );
}

unsigned int
RWMutex::pendingWriterCount() const
{
   return ( mPendingWriterCount );
}
#endif
