#ifndef UTIL_TIMEUTIL_HXX
#define UTIL_TIMEUTIL_HXX

#include <util/util.h>

#ifdef _WIN32
#include <windows.h>
#include <WinSock2.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

#include <ctime>

namespace Util
{

class UTIL_API TimeUtil
{
public:
#ifdef _WIN32
   static void SystemTimeToCTime(SYSTEMTIME& systemTime, struct tm& cTime);
#endif
   static unsigned long long getTimeMs();
   static void getTimeOfDay(struct timeval& now);

};

} // namespace Util

#endif // #ifndef UTIL_TIME_UTIL_HXX
