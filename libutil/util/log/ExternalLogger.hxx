#ifndef UTIL_EXTERNALLOGGER_HXX
#define UTIL_EXTERNALLOGGER_HXX

#include <util/util.h>
#include <sstream>
using namespace std;

#include <util/delegate/delegate.hxx>

namespace Util
{
class UTIL_API ExternalLogger
{
public:
   enum Level
   {
      UnKnown = -1,
      Crit = 2,
      Err = 3,
      Warning = 4,
      Info = 6,
      Debug = 7,
   };

   typedef Util::Delegate<void (const ostringstream&, const char*, int)> LogDelegate; // msg, __FILE__, __LINE__
   typedef Util::Delegate<void (Level, bool&)> LogLevelValidateDelegate; // 

   LogLevelValidateDelegate EnableLogDelegate;
   LogDelegate CritLogDelegate;
   LogDelegate ErrLogDelegate;
   LogDelegate WarningLogDelegate;
   LogDelegate InfoLogDelegate;
   LogDelegate DebugLogDelegate;
#ifndef UTIL_DISABLE_LOGGING
#define ECritLog(msg) \
   do \
   {  \
      bool enableLog = false; \
      EnableLogDelegate(ExternalLogger::Crit, enableLog); \
      if (enableLog) \
      { \
         ostringstream strStrm; \
         strStrm << msg; \
         CritLogDelegate(strStrm, __FILE__, __LINE__); \
      } \
   } while(0)

#define EErrLog(msg) \
   do \
   {  \
      bool enableLog = false; \
      EnableLogDelegate(ExternalLogger::Err, enableLog); \
      if (enableLog) \
      { \
         ostringstream strStrm; \
         strStrm << msg; \
         ErrLogDelegate(strStrm, __FILE__, __LINE__); \
      } \
   } while(0)

#define EWarningLog(msg) \
   do \
   {  \
      bool enableLog = false; \
      EnableLogDelegate(ExternalLogger::Warning, enableLog); \
      if (enableLog) \
      { \
         ostringstream strStrm; \
         strStrm << msg; \
         WarningLogDelegate(strStrm, __FILE__, __LINE__); \
      } \
   } while(0)

#define EInfoLog(msg) \
   do \
   {  \
      bool enableLog = false; \
      EnableLogDelegate(ExternalLogger::Info, enableLog); \
      if (enableLog) \
      { \
         ostringstream strStrm; \
         strStrm << msg; \
         InfoLogDelegate(strStrm, __FILE__, __LINE__); \
      } \
   } while(0)

#define EDebugLog(msg) \
   do \
   {  \
      bool enableLog = false; \
      EnableLogDelegate(ExternalLogger::Debug, enableLog); \
      if (enableLog) \
      { \
         ostringstream strStrm; \
         strStrm << msg; \
         DebugLogDelegate(strStrm, __FILE__, __LINE__); \
      } \
   } while(0)
#else
   #define ECritLog(msg)
   #define EErrLog(msg)
   #define EWarningLog(msg)
   #define EInfoLog(msg)
   #define EDebugLog(msg)
#endif // #ifndef UTIL_DISABLE_LOGGING
};

} // namespace Util

#endif // #ifndef UTIL_EXTERNALLOGGER_HXX
