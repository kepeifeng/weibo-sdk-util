/*

Copyright (c) 2006, Nash Tsai
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the distribution.
* Neither the name of the author nor the names of its
contributors may be used to endorse or promote products derived
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef UTIL_LUASCRIPTLET_HXX
#define UTIL_LUASCRIPTLET_HXX

#include <util/util.h>

#ifdef _WIN32
#  include <windows.h>
#endif

#include <string>
using namespace std;
#include <lua.hpp>

namespace Util
{
   class UTIL_API LuaScriptEngine;

   class UTIL_API LuaScriptlet 
   {	
      lua_State* mThreadState;

      LuaScriptEngine* mEngine;

      void resumeScript(float fParam);

   public:
      enum StatusCode
      {
         lsc_NoError = 0,
      };

      enum ScriptState
      {
         ss_WaitFrame,
         ss_WaitTime,
         ss_Running,
         ss_NotLoaded,
         ss_Done
      };

      ScriptState	mState;
      float			mWaitTimeStamp;	// Time to wake up
      int			mWaitFrame;		// Number of frames to wait
      float			mTime;				// Current time
      int         mCallFuncArgs;

   public: // ctor/dtor
      LuaScriptlet(LuaScriptEngine* pEngine);
      LuaScriptlet(lua_State* L);
      ~LuaScriptlet();

   public: // methods
      //static LuaScriptlet* getScriptObject(lua_State* l);

      lua_State* getLua() { return mThreadState; }

      // Called every frame to update this script
      void update(float fElapsed);

      // Execute a script file
      // @return 0 if no error
      int runFile(const char* filename);

      // Execute script
      // @return 0 if no error
      int runString(const char* commandString, unsigned long dwSize = -1);

#ifdef _WIN32
      // Execute a resource from res DLL
      // @param unloadLib - don't unload to save LoadLibrary time
      // @return 0 if no error
#if 0 // !nash! explicitly state libFileName, and unloadLib
#if !defined(_WEB_TELTEL)
# ifdef _DEBUG
      int runResource(int resId, const char* libFileName = "ResLibrary.dll", bool unloadLib = true);
# else
      int runResource(int resId, const char* libFileName = "ResLibrary.dll", bool unloadLib = false);
# endif
#else
# ifdef _DEBUG
      int runResource(int resId, const char* libFileName = NULL, bool unloadLib = true);
# else
      int runResource(int resId, const char* libFileName = NULL, bool unloadLib = false);
# endif
#endif
#else
      int runResource(int resId, const char* libFileName, bool unloadLib);
#endif

      int runResource(int resId, HMODULE hDll);
#endif
      // Reports or logs script errors
      void error();

      int loadFile(const char* filename);

      class UTIL_API LuaData
      {
         friend class LuaScriptlet;
      public:
         LuaData(LuaScriptlet& luaScriptlet);
         //LuaData(const lua_Number n);
         //LuaData(const lua_Integer n);
         //LuaData(const char *s);
         //LuaData(int b);
         //LuaData(void* p);

         LuaData& operator , (lua_Number n);
         LuaData& operator , (lua_Integer n);
         LuaData& operator , (const char *s);
         LuaData& operator , (bool b);
         LuaData& operator , (void* p);

      protected:

      private:
         LuaScriptlet& mLuaScriptlet;
         //enum DataType
         //{
         //   dt_LuaNum,
         //   dt_LuaInt,
         //   dt_LuaStr,
         //   dt_LuaBool,
         //   dt_LuaUserParam,
         //}

         //map<DataType
      };


      void beginCallFunction(const char* funcName);
      void pushNumberParam(lua_Number n);
      void pushIntegerParam(lua_Integer n);
      void pushStringParam(const char *s);
      void pushLStringParam(const char *s, size_t l);
      const char * pushVFStringParam(const char *fmt, va_list argp);
      const char * pushFStringParam(const char *fmt, ...);
      void pushCClosureParam(lua_CFunction fn, int n);
      void pushCFunctionParam(lua_CFunction fn);
      void pushBoolParam(bool b);
      void pushUserDataParam(void* p);
      void endCallFunction();
      int customEndCallFunction();
      void endCallFunction(lua_Integer& result);
      void endCallFunction(lua_Number& result);
      void endCallFunction(bool& result);
      void endCallFunction(void*& result);
      void endCallFunction(string& result);

      int status() const;
      int yield();

   private: // methods
      int endCallFunctionResultNum(int resultNum);

   private: // members
      int mThreadRefId;
   };

} // namespace Util

#endif // #ifndef UTIL_LUASCRIPTLET_HXX
