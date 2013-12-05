#ifndef UTIL_THREADEDDATA_HXX
#define UTIL_THREADEDDATA_HXX

#include <util/threading/Mutex.hxx>

namespace Util
{

template<typename T, typename _Mutex_Traits = Mutex>
struct ThreadedData
{
public:
   mutable _Mutex_Traits mMutex;
   T mData;
};

} // namespace Util


#endif // #ifndef UTIL_THREADED_DATA_HXX
