#include "stdafx.h"

#include <ostream>
#include <fstream>
#include <cassert>

#include <util/log/Logger.hxx>
#include <util/threading/ThreadIf.hxx>

//#if !defined(_WIN32) || !defined(WIN32)
//#  include <util/log/SysLogStream.hxx>
//#endif

using namespace Util;

boost::scoped_ptr<ostream> GenericLogImpl::sLogger;
unsigned int GenericLogImpl::sLineCount=0;
unsigned int GenericLogImpl::MaxLineCount = 0; // no limit by default
PFileLoggerEvent GenericLogImpl::FileLoggerOpenErrorEvent;
PFileLoggerEvent GenericLogImpl::LogMaxLineCoutEvent;
#ifdef _WIN32
std::wstring GenericLogImpl::sGenericLogFileName;
#else
std::string GenericLogImpl::sGenericLogFileName;
#endif

void
GenericLogImpl::dumpLog(const string& logData, Log::Level level)
{
   bool bHandle = false;

   if (Log::sType & Log::File)
   {
      if (sGenericLogFileName != sLogFileName)
      {
         sGenericLogFileName = sLogFileName;
         if (sLogger)
         {
            sLogger->flush();
            sLogger.reset();
         }
      }
      
      if (!sLogger || (MaxLineCount && sLineCount > MaxLineCount))
      {
         if (MaxLineCount && sLineCount > MaxLineCount)
         {
            LogMaxLineCoutEvent(sLogFileName);
         }

         if (Log::sType & Log::FileAppend)
            sLogger.reset(new (nothrow) ofstream(sGenericLogFileName.c_str(), ios_base::out | ios_base::app));
         else
            sLogger.reset(new (nothrow) ofstream(sGenericLogFileName.c_str(), ios_base::out | ios_base::trunc));
         if (!sLogger->good())
         {
            sLogger.reset();
            FileLoggerOpenErrorEvent(sLogFileName);
         }
         sLineCount = 0;
      }

      if (sLogger)
      {
         (*sLogger) << logData << endl;
         ++sLineCount;
      }
      bHandle = true;
   }

   bool bCerrLogHandled = false;
   if ((level == Log::Err) && (Log::sType & Log::Cerr))
   {
      cerr << logData << endl;
      bCerrLogHandled = true;
      bHandle = true;
   }

   if ((Log::sType & Log::Cout) && !bCerrLogHandled)
   {
      cout << logData << endl;

      bHandle = true;
   }

   if (Log::sType & Log::VSDebugWindow)
   {
      OutputToWin32DebugWindow(logData);
      OutputToWin32DebugWindow("\r\n");
      bHandle = true;
   }

   if (!bHandle)
   {
      assert(0);
      cout << logData << endl;
   }
}

bool
GenericLogImpl::isLogging(Log::Level level)
{
   return (level <= Log::sLevel);
}

void
GenericLogImpl::OutputToWin32DebugWindow(const string& result)
{
#ifdef _WIN32
   const char *text = result.c_str();
   OutputDebugStringA(text);
#endif
}

bool
genericLogCheckLevel(Log::Level level)
{
   return GenericLogImpl::isLogging(level);
}

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
