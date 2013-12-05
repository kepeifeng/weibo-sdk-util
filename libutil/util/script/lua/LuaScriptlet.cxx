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
#include "stdafx.h"

#include <util/script/lua/LuaScriptlet.hxx>
#include <util/script/lua/LuaScriptEngine.hxx>

#include <sstream>
#include <atlbase.h>

using namespace Util;

static const char* printType(lua_State* L, int index)
{
   if (lua_isboolean(L, index))
      return "boolean type";
   else if (lua_iscfunction(L, index))
      return "cfunction type";
   else if (lua_isfunction(L, index))
      return "function type";
   else if (lua_islightuserdata(L, index))
      return "lightuserdata type";
   else if (lua_isnumber(L, index))
      return "number type";
   else if (lua_isstring(L, index))
      return "string type";
   else if (lua_istable(L, index))
      return "table type";
   else if (lua_isthread(L, index))
      return "thread type";
   else if (lua_isuserdata(L, index))
      return "userdata type";
   else if (lua_isnil(L, index))
      return "nil type";
   else
      return "unknown type";
}

LuaScriptlet::LuaScriptlet(LuaScriptEngine* pEngine)
: mEngine(pEngine)
, mState(ss_NotLoaded)
, mWaitTimeStamp(0)
, mWaitFrame(0)
, mTime(0)
, mCallFuncArgs(0)
, mThreadRefId(0)
{
   assert(mEngine);
   // Create the thread state for this script
   mThreadState = lua_newthread(mEngine->getLua());

   // prevent thread from being garbage collected
   lua_pushvalue(mEngine->getLua(), -1);
   mThreadRefId = luaL_ref(mEngine->getLua(), LUA_REGISTRYINDEX);

   //Create a local environment with a link to global environment via __index metamethod
   lua_newtable(mEngine->getLua());
   lua_pushvalue(mEngine->getLua(), -1);
   lua_setmetatable(mEngine->getLua(), -2); //Set itself as metatable
   lua_pushvalue(mEngine->getLua(), LUA_GLOBALSINDEX);
   lua_setfield(mEngine->getLua(), -2, "__index");
   lua_setfenv(mEngine->getLua(), -2);
   lua_pop(mEngine->getLua(), 1);
}

LuaScriptlet::LuaScriptlet(lua_State* L)
: mThreadState(L)
, mEngine(NULL)
, mState(ss_NotLoaded)
, mWaitTimeStamp(0)
, mWaitFrame(0)
, mTime(0)
, mCallFuncArgs(0)
, mThreadRefId(0)
{
   //lua_pushlightuserdata(mThreadState, this);
   //lua_settable(mThreadState, LUA_GLOBALSINDEX);
}

LuaScriptlet::~LuaScriptlet()
{
   if (mEngine)
   {
      int topCount = lua_gettop(mThreadState);
      if (topCount)
         lua_pop(mThreadState, topCount);

      int status = lua_status(mThreadState);
      if (status == 0);
      else if (status == LUA_YIELD)
         assert(false);
      else
         assert(false);
      // unref to allow gc
      luaL_unref(mEngine->getLua(), LUA_REGISTRYINDEX,
         mThreadRefId);
   }
}

void 
LuaScriptlet::update(float fElapsed)
{
   mTime += fElapsed;

   switch (mState)
   {
   case ss_WaitTime:
      if ( mTime >= mWaitTimeStamp )
         resumeScript(0.0f);
      break;

   case ss_WaitFrame:
      --mWaitFrame;
      if ( mWaitFrame <= 0 )
         resumeScript(0.0f);
      break;

   case ss_NotLoaded:
      break;

   default:		
      break;
   }
}

int 
LuaScriptlet::loadFile(const char* filename)
{
   assert( mEngine );
   assert( mEngine->getLua() );
   assert( mThreadState );

   // Here you should check that the script file exists, and 
   // report an error if it does not

   int status = luaL_loadfile(mThreadState, filename);
   if ( status != 0 )
   {
      error();
   }
   return status;
}

int 
LuaScriptlet::runFile(const char* filename)
{
   assert( mEngine );
   assert( mEngine->getLua() );
   assert( mThreadState );

   // Here you should check that the script file exists, and 
   // report an error if it does not

   int status = luaL_loadfile(mThreadState, filename);
   if ( status == 0 )
   {
      resumeScript(0.0f);
   }
   else
   {
      error();
   }
   return status;
}

int 
LuaScriptlet::runString(const char* commandString, unsigned long dwSize)
{
   assert(mEngine);
   assert(mEngine->getLua());
   assert(mThreadState);

   if (dwSize == -1)
   {
      dwSize = strlen(commandString);
   }
   int status = luaL_loadbuffer(mThreadState, commandString, dwSize, "Console");
   if (status == 0)
      status = lua_pcall(mThreadState, lua_gettop(mThreadState)-1, 0, 0);

   if (status)
   {
      error();
      return(-1);
   }

   return (0);
}

#ifdef _WIN32
int
LuaScriptlet::runResource(int resId, const char* libFileName, bool unloadLib)
{
   HMODULE hDll = NULL;
   if (libFileName == NULL)
   {
      char moduleProcessName[MAX_PATH] = {0};
      ::GetModuleFileNameA(_AtlBaseModule.GetModuleInstance(), moduleProcessName, MAX_PATH);
      hDll = ::LoadLibraryA(moduleProcessName);
   }
   else
   {
      hDll = ::LoadLibraryA(libFileName);
   }
   int ret = runResource(resId, hDll);
   if (hDll && unloadLib)
   {
      ::FreeLibrary(hDll);
   }
   return ret;
}

int 
LuaScriptlet::runResource(int resId, HMODULE hDll)
{
   int ret = -1;
   if (hDll != NULL)
   {
      HRSRC hRes = FindResourceA(hDll, MAKEINTRESOURCEA(resId), "Lua_Scripts");
      if (hRes != NULL)
      {
         HGLOBAL hg = ::LoadResource(hDll, hRes);
         if (hg != NULL)
         {
            DWORD dwSize = SizeofResource(hDll, hRes);
            void *p = ::LockResource(hg);
            ret = runString((char*)p, dwSize);
            ::FreeResource(hg);
         }
      }
   }
   return ret;
}
#endif

void 
LuaScriptlet::resumeScript(float fParam)
{
   int status = 0;

   mState = ss_Running;

   // fParam is treated as a return value from the function that yielded
   lua_pushnumber(mThreadState, fParam);
   status = lua_resume(mThreadState, 1);

   if ( status )
   {
      error();
   }
}

void 
LuaScriptlet::error()
{
   const char* msg = lua_tostring(mThreadState, -1);
   if (msg == NULL)
   {
      msg = "(error with no message)";
   }
#ifdef _DEBUG
   ::OutputDebugStringA("LuaScriptlet::error: ");
   ::OutputDebugStringA(msg);
   ::OutputDebugStringA("\n");
#endif
   lua_pop(mThreadState, 1);

   // Display or log the error message here
   
}

// Gets the address of the C++ object managing the specified LUA state
//LuaScriptlet* 
//LuaScriptlet::getScriptObject(lua_State* l)
//{
//   lua_pushlightuserdata(l, l);
//   lua_gettable(l, LUA_GLOBALSINDEX );
//   return((LuaScriptlet*)lua_touserdata(l, -1));
//}


void 
LuaScriptlet::beginCallFunction(const char* funcName)
{
   lua_getglobal(mThreadState, funcName); /* function to be called */
   mCallFuncArgs = 0;
}

// private
int 
LuaScriptlet::endCallFunctionResultNum(int resultNum)
{
   int status = lua_pcall(mThreadState, mCallFuncArgs, resultNum, 0);     /* call lua function with argNum arguments and resultNum result */
   if (status != 0)
   {
      error();
   }
   mCallFuncArgs = 0;
   return status;
}

void
LuaScriptlet::endCallFunction()
{
   endCallFunctionResultNum(0);
   int resultNum = lua_gettop(mThreadState);
   if (resultNum > 0)
   {
      lua_pop(mThreadState, resultNum);
   }
}

int
LuaScriptlet::customEndCallFunction()
{
   endCallFunctionResultNum(LUA_MULTRET);
   return lua_gettop(mThreadState);
}

void 
LuaScriptlet::endCallFunction(lua_Integer& result)
{
   if (endCallFunctionResultNum(1) == 0)
   {
      int resultNum = lua_gettop(mThreadState);
      if (resultNum > 0 && lua_isnumber(mThreadState, -1))
      {
         result = lua_tointeger(mThreadState, -1);
         lua_pop(mThreadState, 1);
         --resultNum;
      }
      if (resultNum)
         lua_pop(mThreadState, resultNum);
   }
}

void 
LuaScriptlet::endCallFunction(lua_Number& result)
{
   if (endCallFunctionResultNum(1) == 0)
   {
      int resultNum = lua_gettop(mThreadState);
      if (resultNum > 0 && lua_isnumber(mThreadState, -1))
      {
         result = lua_tonumber(mThreadState, -1);
         lua_pop(mThreadState, 1);
         --resultNum;
      }
      if (resultNum)
         lua_pop(mThreadState, resultNum);
   }
}

void 
LuaScriptlet::endCallFunction(bool& result)
{
   if (endCallFunctionResultNum(1) == 0)
   {
      int resultNum = lua_gettop(mThreadState);
      if (resultNum > 0 && lua_isboolean(mThreadState, -1))
      {
         result = (bool)lua_toboolean(mThreadState, -1);
         lua_pop(mThreadState, 1);
         --resultNum;
      }
      if (resultNum)
         lua_pop(mThreadState, resultNum);
   }
}

void
LuaScriptlet::endCallFunction(void*& result)
{
   if (endCallFunctionResultNum(1) == 0)
   {
      int resultNum = lua_gettop(mThreadState);
      if (resultNum > 0 && lua_isuserdata(mThreadState, -1))
      {
         result = lua_touserdata(mThreadState, -1);
         lua_pop(mThreadState, 1);
         --resultNum;
      }
      if (resultNum)
         lua_pop(mThreadState, resultNum);
   }
}

void
LuaScriptlet::endCallFunction(string& result)
{
   if (endCallFunctionResultNum(1) ==0)
   {
      int resultNum = lua_gettop(mThreadState);
      if (resultNum > 0 && lua_isstring(mThreadState, -1))
      {
         result = lua_tolstring(mThreadState, -1, NULL);
         lua_pop(mThreadState, 1);
         --resultNum;
      }
      if (resultNum)
         lua_pop(mThreadState, resultNum);
   }
}

void 
LuaScriptlet::pushNumberParam(lua_Number n)
{
   lua_pushnumber(mThreadState, n);
   ++mCallFuncArgs;
}

void 
LuaScriptlet::pushIntegerParam(lua_Integer n)
{
   lua_pushinteger(mThreadState, n);
   ++mCallFuncArgs;
}

void 
LuaScriptlet::pushStringParam(const char *s)
{
   lua_pushstring(mThreadState, s);
   ++mCallFuncArgs;
}

void 
LuaScriptlet::pushLStringParam(const char *s, size_t l)
{
   lua_pushlstring(mThreadState, s, l);
   ++mCallFuncArgs;
}

const char * 
LuaScriptlet::pushVFStringParam(const char *fmt, va_list argp)
{
   return lua_pushvfstring(mThreadState, fmt, argp);
   ++mCallFuncArgs;
}

const char * 
LuaScriptlet::pushFStringParam(const char *fmt, ...)
{
   va_list vaList;
   va_start(vaList, fmt);
   int n = ::vprintf(fmt, vaList);
   va_end(vaList);
   if (n >= 0)
      return pushVFStringParam(fmt, vaList);
   return NULL;
}

void 
LuaScriptlet::pushCClosureParam(lua_CFunction fn, int n)
{
   lua_pushcclosure(mThreadState, fn, n);
   ++mCallFuncArgs;
}

void 
LuaScriptlet::pushCFunctionParam(lua_CFunction fn)
{
   pushCClosureParam(fn, 0);
   ++mCallFuncArgs;
}

void 
LuaScriptlet::pushBoolParam(bool b)
{
   lua_pushboolean(mThreadState, b);
   ++mCallFuncArgs;
}

void 
LuaScriptlet::pushUserDataParam(void* p)
{
   lua_pushlightuserdata(mThreadState, p);
   ++mCallFuncArgs;
}

int 
LuaScriptlet::status() const
{
   return lua_status(mThreadState);
}

int 
LuaScriptlet::yield()
{
   return lua_yield(mThreadState, 0);
}

LuaScriptlet::LuaData::LuaData(LuaScriptlet& luaScriptlet)
:mLuaScriptlet(luaScriptlet)
{
}

LuaScriptlet::LuaData& 
LuaScriptlet::LuaData::operator , (lua_Number n)
{
   mLuaScriptlet.pushNumberParam(n);
   return *this;
}

LuaScriptlet::LuaData&
LuaScriptlet::LuaData::operator , (lua_Integer n)
{
   mLuaScriptlet.pushIntegerParam(n);
   return *this;
}

LuaScriptlet::LuaData& 
LuaScriptlet::LuaData::operator , (const char *s)
{
   mLuaScriptlet.pushStringParam(s);
   return *this;
}

LuaScriptlet::LuaData&
LuaScriptlet::LuaData::operator , (bool b)
{
   mLuaScriptlet.pushBoolParam(b);
   return *this;
}

LuaScriptlet::LuaData&
LuaScriptlet::LuaData::operator , (void* p)
{
   mLuaScriptlet.pushUserDataParam(p);
   return *this;
}
