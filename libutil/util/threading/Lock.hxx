#ifndef UTIL_LOCK_HXX
#define UTIL_LOCK_HXX  

#include <util/threading/Mutex.hxx>
#include <util/threading/RWMutex.hxx>

#define synchronized(M)  for(Util::Lock M##_lock(M); M##_lock.isLock(); M##_lock.unlock())
#define rsynchronized(M)  for(Util::RLock M##_lock(M); M##_lock.isLock(); M##_lock.unlock())
#define wsynchronized(M)  for(Util::WLock M##_lock(M); M##_lock.isLock(); M##_lock.unlock())

namespace Util
{

/**
 * Base lock template class
 *
 * @author Jinti, modified by Nash
 */
template <typename _Mutex_Traits, typename _Lock_Traits>
class BaseLock
{
protected:
   _Mutex_Traits& mMutex;
   bool mIsLock;

public:    
   BaseLock(_Mutex_Traits& mtx):
      mMutex(mtx),
      mIsLock(true)
      {
         _Lock_Traits::lock(mMutex);
      }

      ~BaseLock()
      {
         unlock();
      }

public:
   inline void lock()
   {
      if (!mIsLock)
      {
         mIsLock = true;
         _Lock_Traits::lock(mMutex);
      }
   }

   inline void unlock()
   {
      if (mIsLock)
      {
         mIsLock = false;
         _Lock_Traits::unlock(mMutex);
      }
   }

   inline bool isLock() const { return mIsLock; }
};

class ReadLockTraits
{
public:
   inline static void lock(RWMutex& mtx)
   {
      mtx.readlock();
   }

   inline static void unlock(RWMutex& mtx)
   {
      mtx.unlock();
   }
};

class WriteLockTraits
{
public:
   inline static void lock(RWMutex& mtx)
   {
      mtx.writelock();
   }

   inline static void unlock(RWMutex& mtx)
   {
      mtx.unlock();
   }
};

class LockTraits
{
public:
   inline static void lock(Mutex& mtx)
   {
      mtx.lock();
   }

   inline static void unlock(Mutex& mtx)
   {
      mtx.unlock();
   }
};

typedef BaseLock<RWMutex, ReadLockTraits> RLock;
typedef BaseLock<RWMutex, WriteLockTraits> WLock;
typedef BaseLock<Mutex, LockTraits> Lock;

} // namespace Util

#endif // #ifndef UTIL_LOCK_HXX


