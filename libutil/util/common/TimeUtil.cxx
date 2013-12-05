#include "stdafx.h"

#include <util/common/TimeUtil.hxx>
#include <boost/throw_exception.hpp>

#if !_HAS_EXCEPTIONS && defined(_WIN32)
#include <typeinfo>
namespace std 
{
   using ::type_info;
}
#endif

#include <cassert>

using namespace Util;

#ifdef _WIN32
void TimeUtil::SystemTimeToCTime(SYSTEMTIME& systemTime, struct tm& cTime)
{
   ::ZeroMemory(&cTime, sizeof(struct tm));
   cTime.tm_hour = systemTime.wHour;
   cTime.tm_min = systemTime.wMinute;
   cTime.tm_sec = systemTime.wSecond;
   cTime.tm_mon = systemTime.wMonth - 1;
   cTime.tm_mday = systemTime.wDay;
   cTime.tm_wday = systemTime.wDayOfWeek;
   cTime.tm_year = systemTime.wYear - 1900;
}
#endif

void 
TimeUtil::getTimeOfDay(struct timeval& now)
{
#ifdef _WIN32
   struct __timeb64 timebuffer;
   _ftime64(&timebuffer);
   now.tv_sec = timebuffer.time;
   now.tv_usec = timebuffer.millitm * 1000;
#else
   gettimeofday(&now, NULL);
#endif
}

unsigned long long
TimeUtil::getTimeMs()
{
   struct timeval now;
   getTimeOfDay(now);
   unsigned long long time = 0;
   time = now.tv_sec;
   time *= 1000000;
   time += now.tv_usec;
   time /= 1000;

   return time;
}

