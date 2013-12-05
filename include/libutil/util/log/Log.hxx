#ifndef UTIL_LOG_HXX
#define UTIL_LOG_HXX

#include <util/util.h>
#include <boost/scoped_ptr.hpp>

#include <set>
#include <iostream>
#include <sstream>
#include <map>
using namespace std;


#ifndef _WIN32
#include <syslog.h>
#include <unistd.h>
#include <pthread.h>
#endif

#include <util/log/Subsystem.hxx>
#include <util/threading/Mutex.hxx>

namespace Util
{
   class ExternalLoggerForLog;

   class UTIL_API Log
   {
   public:
      typedef enum 
      {
         Cout = 1,
         Cerr = 1 << 1,
         File = 1 << 3,
         FileAppend = 1 << 4,
         VSDebugWindow = 1 << 7,   // Use only for Visual Studio Debug Window logging - WIN32 must be defined
      } Type;

      typedef enum
      {
         Unknown = -1,
#ifdef _WIN32
         Crit = 2,
         Err = 3,
         Warning = 4,
         Info = 6,
         Debug = 7,
#else
         Crit = LOG_CRIT,
         // #ifdef ERR // ncurses defines a macro called ERR 
         //          SIP2_ERR = LOG_ERR,
         // #else
         //          ERR = LOG_ERR,
         // #endif
         Err,
         Warning = LOG_WARNING,
         Info = LOG_INFO,
         Debug = LOG_DEBUG,
#endif
         Stack = 8,
         StdErr = 9,
         Bogus = 666
      } Level;

      /**
      Implementation for logging macros.
      Log::Guard(Log::Info, Subsystem::TEST, __FILE__, __LINE__) << ... ;
      */
      class UTIL_API Guard
      {
      public:
         /** Remember the logging values and be a a stream to receive
         the log contents. */
         Guard(Level level,
            const char* system,
            const char* file,
            int line);

         /** Commit logging */
         ~Guard();

         ostringstream& asStream() {return mStream;}
         operator ostringstream&() {return mStream;}

      private:
         Log::Level mLevel;
         const char* mSubsystem;
         const char* mFile;
         int mLine;
         string mData;
         ostringstream mStream;
         bool mHasCustomMsg;
         Guard& operator=(const Guard&);
      };
#ifdef _DEBUG
     static void setFormatMsg(const std::string& msg);
     static std::string getFormatMsg();
#endif 

#ifdef _WIN32
      static bool getModuleFileName(std::wstring& moduleFileName);
#endif
      static bool getModuleFileName(std::string& moduleFileName);

      template<typename CharType, typename StringType>
      static bool extractModuleName(const CharType* keyWord, StringType& moduleFileName);
      /// output the loglevel, hostname, appname, pid, tid, subsystem
	  static void formatPattern(Log::Level level,
		  const Subsystem& subsystem,
		  const char* pfile,
		  int line,
		  std::string& outFormattedMsg,
		  bool& outHasCustomMsg);

      static void tags(Log::Level level,
         const char* subsystem,
         const char* file,
         int line,
         std::string& outFormattedMsg,
         bool& outHasCustomMsg);
      static string& timestamp(string& result, const string& strFormat);
      static string timestamp();
      static string getTimeZoneInfo();
      static ExternalLoggerForLog* getExternal()
      {
         return sExternalLogger;
      }

      static string getAppName()
      {
         return sAppName;
      }

      static void initialize(int type, // such as "Cout" or "Cout | VSDebugWindow"
         Level level,
         const char * appName,
#ifdef _WIN32
         const wchar_t* logFileName = 0,
#else
         const char* logFileName = 0,
#endif
         ExternalLoggerForLog* externalLogger = 0);

      static void setLevel(Level level);
      static Level level() { return sLevel; }
      static Level toLevel(const char* level);
      static Type toType(const char* type);
      static string toString(Level l);
      
      static Mutex sMutex;

      static int sType;
      static const string delim;
   protected:
      static Level sLevel;
      static string sAppName;
#ifdef _WIN32
      static wstring sLogFileName;
#else
      static string sLogFileName;
#endif
      static ExternalLoggerForLog* sExternalLogger;
      static string sPattern;
      static string sTimeZone;

#ifndef _WIN32
      static pid_t sPid;
#else
      static int sPid;
#endif
      static const char* sDescriptions[];
      static map<int, Level> sServiceToLevel;
      static string mFormatMsg;
      ExternalLoggerForLog* mExternalLogger;
   };

   /** Interface functor for external logging. */
   class UTIL_API ExternalLoggerForLog
   {
   public: // dtor
      virtual ~ExternalLoggerForLog() {}
   public:
      /** return true to also do default logging, false to supress default logging. */
      virtual bool operator()(Log::Level level,
         const Subsystem& subsystem, 
         const string& appName,
         const char* file,
         int line,
         const string& message) = 0;
   };

}

#endif // #ifndef UTIL_LOG_HXX

/* ====================================================================
* The Vovida Software License, Version 1.0
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
* 
* 3. The names "VOCAL", "Vovida Open Communication Application Library",
*    and "Vovida Open Communication Application Library (VOCAL)" must
*    not be used to endorse or promote products derived from this
*    software without prior written permission. For written
*    permission, please contact vocal@vovida.org.
*
* 4. Products derived from this software may not be called "VOCAL", nor
*    may "VOCAL" appear in their name, without prior written
*    permission of Vovida Networks, Inc.
* 
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
* NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
* NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
* IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
* 
* ====================================================================
* 
* This software consists of voluntary contributions made by Vovida
* Networks, Inc. and many individuals on behalf of Vovida Networks,
* Inc.  For more information on Vovida Networks, Inc., please see
* <http://www.vovida.org/>.
*
*/
