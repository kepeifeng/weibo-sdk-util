#ifndef UTIL_LOGGER_HXX
#define UTIL_LOGGER_HXX 

#include <util/util.h>

#include <util/log/Log.hxx>
#include <util/threading/Lock.hxx>
#include <util/delegate/delegate.hxx>

#include <iosfwd>

#ifdef _WIN32
# include <windows.h>
#endif 


/**
Defines a set of logging macros, one for each level of logging.

Example:
#include Logger.hxx
#define DEFAULT_SUBSYSTEM resip::Subsystem::SIP
...
DebugLog(<< "hi there " << mix << 4 << types);  // note leading << and no endl
*/

#define DELIM

#ifndef UTIL_DISABLE_LOGGING

#define StackLog(args_) \
   GenericLog(DEFAULT_SUBSYSTEM, Util::Log::Stack, args_)

#define DebugLog(args_) \
   GenericLog(DEFAULT_SUBSYSTEM, Util::Log::Debug, args_)

#define InfoLog(args_) \
   GenericLog(DEFAULT_SUBSYSTEM, Util::Log::Info, args_)

#define WarningLog(args_) \
   GenericLog(DEFAULT_SUBSYSTEM, Util::Log::Warning, args_)

#define ErrLog(args_) \
   GenericLog(DEFAULT_SUBSYSTEM, Util::Log::Err, args_)

#define CritLog(args_) \
   GenericLog(DEFAULT_SUBSYSTEM, Util::Log::Crit, args_)

#define StackLogV2(args_, subsystem_) \
   GenericLog(subsystem_, Util::Log::Stack, args_)

#define DebugLogV2(args_, subsystem_) \
   GenericLog(subsystem_, Util::Log::Debug, args_)

#define InfoLogV2(args_, subsystem_) \
   GenericLog(subsystem_, Util::Log::Info, args_)

#define WarningLogV2(args_, subsystem_) \
   GenericLog(subsystem_, Util::Log::Warning, args_)

#define ErrLogV2(args_, subsystem_) \
   GenericLog(subsystem_, Util::Log::Err, args_)

#define CritLogV2(args_, subsystem_) \
   GenericLog(subsystem_, Util::Log::Crit, args_)

// do/while allows a {} block in an expression
#define GenericLog(system_, level_, args_)   \
do \
{ \
   if (genericLogCheckLevel(level_)) \
   { \
      Util::Log::Guard _resip_log_guard(level_, system_, __FILE__, __LINE__); \
      _resip_log_guard.asStream() args_; \
   } \
} while (0)

#ifdef NO_DEBUG
// Suppress debug logging at compile time
#undef DebugLog(args_)
#define DebugLog(args_)
#undef StackLog(args_)
#define StackLog(args_)
#undef DebugLogV2(args_, subsystem_)
#define DebugLogV2(args_, subsystem_)
#undef StackLogV2(args_)
#define StackLogV2(args_, subsystem_)
#endif

#else
#define CerrLog(args_)

#define StackLog(args_)

#define DebugLog(args_)

#define InfoLog(args_)

#define WarningLog(args_)

#define ErrLog(args_)

#define CritLog(args_)

#define StackLogV2(args_, subsystem_)

#define DebugLogV2(args_, subsystem_)

#define InfoLogV2(args_, subsystem_)

#define WarningLogV2(args_, subsystem_)

#define ErrLogV2(args_, subsystem_)

#define CritLogV2(args_, subsystem_) 

#endif //#ifndef UTIL_DISABLE_LOGGING

bool UTIL_API genericLogCheckLevel(Util::Log::Level level);

namespace Util
{
#ifdef _WIN32
   typedef Util::Delegate<void (const wstring&)> PFileLoggerEvent;
#else
   typedef Util::Delegate<void (const string&)> PFileLoggerEvent;
#endif

   class UTIL_API GenericLogImpl :  public Log 
   {
   public:
      static bool isLogging(Log::Level level) ;
      static unsigned int MaxLineCount;
      static void OutputToWin32DebugWindow(const string& result);
      static void dumpLog(const string& logData, Util::Log::Level level);

   public:
      static PFileLoggerEvent FileLoggerOpenErrorEvent;
      static PFileLoggerEvent LogMaxLineCoutEvent;

   private:
      static boost::scoped_ptr<ostream> sLogger;
      static unsigned int sLineCount;
#ifdef _WIN32
      static std::wstring sGenericLogFileName;
#else
      static std::string sGenericLogFileName;
#endif
   };

}

#endif // #ifndef UTIL_LOGGER_HXX

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
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
