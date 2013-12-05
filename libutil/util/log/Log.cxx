#include "stdafx.h"

#include <cassert>
#include <iostream>
#include <ctime>
#include <vector>
#include <cstring>
#include <stdio.h>

#include <boost/config.hpp>

#ifndef _WIN32
#include <sys/time.h>
#include <cstdlib>
#include <unistd.h>
#else
#include <WinSock2.h>
#endif
#include <sys/types.h>

#include <util/log/Log.hxx>
#include <util/log/Logger.hxx>
#include <util/threading/Lock.hxx>
#include <util/common/StlUtil.hxx>
#include <util/compat.hxx>

#if !defined(_WIN32)
//#include <linux/limits.h>
#include <limits.h>
#endif //

using namespace Util;

#ifdef _WIN32
#define DEFAUL_LOG_FILE_NAME L"UTIL_LOG"
#else
#define DEFAUL_LOG_FILE_NAME "UTIL_LOG"
#endif

#define DEFAUL_LOG_APP_NAME "UTIL"

const string Log::delim(" | ");
Log::Level Log::sLevel = Log::Info;
int Log::sType = Cout;
string Log::sAppName;
#ifdef _WIN32
wstring Log::sLogFileName;
#else
string Log::sLogFileName;
#endif
string Log::sTimeZone;
ExternalLoggerForLog* Log::sExternalLogger = 0;

string Log::mFormatMsg = "";

#ifdef _WIN32
int Log::sPid=0;
#else
pid_t Log::sPid=0;
#endif


//volatile short Log::touchCount = 0;

#ifndef _WIN32
#  ifndef MAX_PATH
#     define MAX_PATH 1024
#  endif
#endif

map<int, Log::Level> Log::sServiceToLevel;

const char*
Log::sDescriptions[] = {"NONE", "EMERG", "ALERT", "CRIT", "ERR", "WARNING", "NOTICE", "INFO", "DEBUG", "STACK", "CERR", ""};

Mutex Log::sMutex;

inline bool isEqualNoCase(const char* left, const char* right)
{
   return ( (strlen(left) == strlen(left)) &&
      (strncasecmp(left, right, strlen(left)) == 0) );
}

void Log::formatPattern(Log::Level level,
						const Subsystem& subsystem,
						const char* pfile,
						int line,
						std::string& formatMsg,
						bool& outHasCustomMsg)
{
	Lock lock(Log::sMutex);

	try
	{
		formatMsg.reserve(256);
		formatMsg.clear();

		//add time stamp
		string strFormat = "%Y/%m/%d %H:%M:%S.%s";
		Log::timestamp(formatMsg, strFormat);
		formatMsg += " ";

		//add level name
		formatMsg += sDescriptions[level+1];
		formatMsg += " ";

		//add app name
		formatMsg += "[APP:";
		formatMsg += sAppName;
		formatMsg += "] ";

		//add line num
		ostringstream stm;
		stm << "{ " << pfile << ":" << line << " }";
		formatMsg += stm.str();
		formatMsg += " ";

		//add thread id
# if !defined(HAVE_LIBPTHREAD) && defined(_WIN32)
		DWORD dwThreadid = GetCurrentThreadId();
# else
		void* dwThreadid = 0;//pthread_self().p;
# endif
		ostringstream stm2;
		stm2 << "[THREADID:" << dwThreadid << "]";
		formatMsg += stm2.str();
		formatMsg += " ";

		//add sub name
		formatMsg += "[SUBSYSTEM:";
		formatMsg += subsystem.getSubsystem();
		formatMsg += "] ";

		//add msg
		formatMsg += "%M";
		formatMsg += " ";
		outHasCustomMsg = true;
	}
	catch (exception& e)
	{
		formatMsg += " : ";
		formatMsg += e.what();
	}
	catch (...)
	{
	}
}

void 
Log::initialize(int type, 
                Level level, 
                const char* appName,
#ifdef _WIN32
                const wchar_t* logFileName,
#else
                const char* logFileName,
#endif
                ExternalLoggerForLog* externalLogger)
{
   Lock lock(Log::sMutex);

   sTimeZone = getTimeZoneInfo();

   sType = type;
   sLevel = level;

   if (StringUtil::NotNullOrEmpty(logFileName))
   {
      sLogFileName = logFileName;
   }
   else
   {
      if (!getModuleFileName(sLogFileName))
      {
         sLogFileName = DEFAUL_LOG_FILE_NAME;
      }
#ifdef _WIN32
      sLogFileName += L".log";
#else
      sLogFileName += ".log";
#endif
   }
   sExternalLogger = externalLogger;

   if (StringUtil::NotNullOrEmpty(appName))
   {
      string copy(appName);
      string::size_type pos = copy.find_last_of('/');
      if ( pos == string::npos || pos == copy.size())
      {
         sAppName = appName;
      }
      else
      {
         sAppName = std::string((copy.substr(pos+1).c_str()));
      }
   }
   else if (!getModuleFileName(sAppName))
   {
      sAppName = DEFAUL_LOG_APP_NAME;
   }
#ifndef MAX_PATH
#  define MAX_PATH 256
#endif
#ifdef _WIN32 
   sPid = (int)GetCurrentProcess();
#else
   sPid = getpid();
#endif
}

template<typename CharType, typename StringType>
bool Log::extractModuleName(const CharType* keyWord, StringType& moduleFileName)
{
   bool result = false;

   typename StringType::size_type foundPos = moduleFileName.rfind(keyWord);  // extract ModuleName, Such as : c:\\test\\test.exe => test.exe
   if (foundPos != std::string::npos)
   {
      moduleFileName = moduleFileName.substr(foundPos + 1);
      result = true;
   }

   return result;
}

bool Log::getModuleFileName(std::string& moduleFileName)
{
   bool result = false;

   char moduleName[MAX_PATH] = {0};
#ifdef _WIN32
   unsigned long long dwCode = ::GetModuleFileNameA(NULL, moduleName, MAX_PATH);
   if (dwCode > 0 && moduleName[0])
   {
      moduleFileName = moduleName;
      result = extractModuleName("\\", moduleFileName);
   }
#else
   moduleName[0] = '\0';
   unsigned long long dwCode = readlink("/proc/self/exe", moduleName, PATH_MAX);
   if (dwCode > 0 && dwCode < PATH_MAX && moduleName[0] != '\0' ) 
   {
      moduleName[dwCode] = '\0';
      moduleFileName = moduleName;
      result = extractModuleName("/", moduleFileName);
   }
#endif
   return result;
}

#ifdef _WIN32
bool Log::getModuleFileName(std::wstring& moduleFileName)
{
   bool result = false;

   wchar_t moduleName[MAX_PATH] = {0};
   unsigned long long dwCode = ::GetModuleFileNameW(NULL, moduleName, MAX_PATH);
   if (dwCode > 0 && moduleName[0])
   {
      moduleFileName = moduleName;
      extractModuleName(L"\\", moduleFileName);
   }

   return result;
}
#endif

void Log::setLevel(Level level)
{
   Lock lock(sMutex);
   sLevel = level;
}

const static string log_("LOG_");

string
Log::toString(Level l)
{
   return log_ + sDescriptions[l+1];
}

Log::Level
Log::toLevel(const char* level)
{
   string pri = StringUtil::getNotNullString(level);
   if (pri.find("LOG_", 0) == 0)
   {
      size_t erSize = 4;
      pri.erase(0, erSize);
   }

   int i=0;
   while (sDescriptions[i][0] != '\0')
   {
      if (::strcasecmp(pri.c_str(), sDescriptions[i]) == 0)
      {
         return Level(i-1);
      }
      ++i;
   }

   cerr << "Choosing Debug level since string was not understood: " << level << endl;
   return Log::Debug;
}

Log::Type
Log::toType(const char* type)
{
   if (StringUtil::NotNullOrEmpty(type))
   {
      if (::strcasecmp(type, "cerr") == 0)
      {
         return Log::Cerr;
      }
      else if (::strcasecmp(type, "file") == 0)
      {
         return Log::File;
      }
   }

   return Log::Cout;
}

void
Log::tags(Log::Level level,
          const char* subsystem,
          const char* pfile,
          int line,
          std::string& outFormattedMsg,
          bool& outHasCustomMsg)
{
   const char* file = pfile + strlen(pfile);
   while (file != pfile &&
      (*file != '\\' || *file != '/'))
   {
      --file;
   }
   if (file != pfile)
   {
      ++file;
   }
   formatPattern(level, subsystem, file, line, outFormattedMsg, outHasCustomMsg);
}

string
Log::timestamp()
{
   char buffer[256] = { 0 };
   string result(buffer);
   return timestamp(result, "");
}

string&
Log::timestamp(string& res, const string& strTimeFormat) 
{
   res.reserve(255);
   char* datebuf = const_cast<char*>(res.data());
   const unsigned int datebufSize = 255;
   res.clear();

#ifdef _WIN32 
   int result = 1; 
   SYSTEMTIME systemTime;
   struct timeval tv = {0,0};
   time((time_t*)&tv.tv_sec);
   GetLocalTime(&systemTime);
   tv.tv_usec = systemTime.wMilliseconds * 1000;   
#else 
   struct timeval tv; 
   int result = gettimeofday (&tv, NULL);
#endif

   string strFormat(strTimeFormat);
   if (strFormat.empty())
   {
      strFormat = "%Y/%m/%d %H:%M:%S.%s";
   }

   bool hasMS = false;
   std::string::size_type msFound = strFormat.find("%s");
   if (std::string::npos != msFound)
   {
      strFormat.erase(msFound, msFound + 1);
      hasMS = true;
   }

   if (result == -1)
   {
      /* If we can't get the time of day, don't print a timestamp.
      Under Unix, this will never happen:  gettimeofday can fail only
      if the timezone is invalid which it can't be, since it is
      uninitialized]or if tv or tz are invalid pointers. */
      datebuf [0] = 0;
   }
   else
   {
      /* The tv_sec field represents the number of seconds passed since
      the Epoch, which is exactly the argument gettimeofday needs. */
      const time_t timeInSeconds = (time_t) tv.tv_sec;
      strftime (datebuf,
         datebufSize,
         strFormat.c_str(), /* guaranteed to fit in 256 chars,
                            hence don't check return code */
                            localtime (&timeInSeconds));
   }

   if (hasMS)
   {
      char msbuf[5];
      /* Dividing (without remainder) by 1000 rounds the microseconds
      measure to the nearest millisecond. */
      sprintf(msbuf, "%3.3ld", long(tv.tv_usec / 1000));

      int datebufCharsRemaining = datebufSize - strlen (datebuf);
      strncat (datebuf, msbuf, datebufCharsRemaining - 1);
   }

   datebuf[datebufSize - 1] = '\0'; /* Just in case strncat truncated msbuf,
                                    thereby leaving its last character at
                                    the end, instead of a null terminator */

   res = datebuf;
   res += sTimeZone;
   // ugh, resize the Data
   //res.at(strlen(datebuf)-1);

   return res;
}

string 
Log::getTimeZoneInfo()
{
#if defined(_WIN32)
   TIME_ZONE_INFORMATION timeZoneInfo;
   ::ZeroMemory(&timeZoneInfo, sizeof(TIME_ZONE_INFORMATION));
   DWORD dwRet = GetTimeZoneInformation(&timeZoneInfo);

   float timezone = 0.0;
   if (dwRet == TIME_ZONE_ID_STANDARD || dwRet == TIME_ZONE_ID_UNKNOWN)
   {
      if (timeZoneInfo.StandardName != NULL)
      {
         timezone = (float)timeZoneInfo.Bias / -60.0f;
      }
   }
   else if (dwRet == TIME_ZONE_ID_DAYLIGHT)
   {
      timezone = (float)(timeZoneInfo.Bias + timeZoneInfo.DaylightBias) / -60.0f;
   }

   if (timezone == 0.0)
   {
      return StringUtil::sEmpty;
   }
   else
   {
      ostringstream ostrStream;
      if (timezone  > 0.0)
      {
         ostrStream << "+";
      }
      ostrStream << timezone;

      return ostrStream.str();
   }
#else
   return StringUtil::sEmpty;
#endif
}

#ifdef _DEBUG
void Log::setFormatMsg(const std::string& msg)
{
   mFormatMsg = msg;
}

std::string Log::getFormatMsg()
{
   return mFormatMsg;
}
#endif

Log::Guard::Guard(Log::Level level,
                  const char* subsystem,
                  const char* file,
                  int line) :
mLevel(level),
mSubsystem(subsystem),
mFile(file),
mLine(line),
mHasCustomMsg(false)
{
   Log::tags(mLevel, mSubsystem, mFile, mLine, mData, mHasCustomMsg);
}

Log::Guard::~Guard()
{
   if (mHasCustomMsg)
   {
      string::size_type nPos = mData.find("%M");
      if (string::npos != nPos)
      {
         mData.replace(nPos, 2, mStream.str());
         mStream.str("");
      }
   }
#ifdef _DEBUG
   Log::setFormatMsg(mData.c_str());
#endif
   if (Log::getExternal())
   {
      if (!(*Log::getExternal())(mLevel,
         mSubsystem,
         Log::getAppName(),
         mFile,
         mLine,
         mData))
      {
         return;
      }
   }

   Lock lock(Util::Log::sMutex);
   GenericLogImpl::dumpLog(mData, mLevel);
}


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
