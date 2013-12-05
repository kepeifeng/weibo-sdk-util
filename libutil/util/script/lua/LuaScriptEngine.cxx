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

#include <util/script/lua/LuaScriptEngine.hxx>
#include <util/script/lua/LuaScriptlet.hxx>

#include <util/threading/Lock.hxx>
#include <util/common/StlUtil.hxx>

#include <utility>
#include <algorithm>


using namespace Util;

//auto_ptr<LuaScriptEngine> LuaScriptEngine::sScriptEngine;
//static 
LuaScriptEngine::ScriptEngineMap LuaScriptEngine::sScriptEngines;

//static 
Mutex LuaScriptEngine::sScriptEnginesMutex;

LuaScriptEngine& LuaScriptEngine::getScriptEngine()
{
   Lock guard(sScriptEnginesMutex);
#ifdef _WIN32
   unsigned long currentThreadId = ::GetCurrentThreadId();
#else
   assert(!"yet to implement");
#endif
   ScriptEngineMap::iterator found =
      sScriptEngines.find(currentThreadId);
   if (found != sScriptEngines.end())
   {
      return *(found->second);
   }
   else
   {
      pair<ScriptEngineMap::iterator,bool> inserted = 
         sScriptEngines.insert(make_pair(currentThreadId, boost::shared_ptr<LuaScriptEngine>(new (nothrow) LuaScriptEngine())));
      assert(inserted.second);
      return *(inserted.first->second);
   }
}
// static
void 
LuaScriptEngine::releaseScriptEngine()
{
   Lock guard(sScriptEnginesMutex);
#ifdef _WIN32
   unsigned long currentThreadId = ::GetCurrentThreadId();
#else
   assert(!"yet to implement");
#endif
   sScriptEngines.erase(currentThreadId);
   //sScriptEngine.reset();
}
// static
void 
LuaScriptEngine::releaseScriptEngines()
{
   Lock guard(sScriptEnginesMutex);
   sScriptEngines.clear();
}

// Add any LUA libraries that need to be loaded here, this
// may include user created libs
static const luaL_reg lualibs[] =
{
   {"base",	luaopen_base},	
   //{LUA_IOLIBNAME, luaopen_io},
   //{LUA_LOADLIBNAME, luaopen_package},
   {LUA_MATHLIBNAME, luaopen_math},
   {LUA_OSLIBNAME, luaopen_os},
   {LUA_STRLIBNAME, luaopen_string},
   {LUA_TABLIBNAME, luaopen_table},
   {LUA_DBLIBNAME, luaopen_debug},
   //{"system",		LUAOpenSystemLib},	
   { NULL,		NULL }
};

// This opens all the LUA libraries declared above
void OpenLUALibs(lua_State *l)
{
   const luaL_reg *lib;

   for (lib = lualibs; lib->func != NULL; ++lib)
   {
      lib->func(l);
      lua_settop(l, 0);
   }
}


LuaScriptEngine::LuaScriptEngine()
{
   mLuaState = lua_open();
   assert(mLuaState);

   OpenLUALibs(mLuaState);

   // This is where all the classes that use LUA are registered,
   // I intentionally put this fucntion definition in a separate file called 
   // LUARegistry.cpp for easy access
   //registerScriptClasses();
}

LuaScriptEngine::~LuaScriptEngine(void)
{
   if ( mScriptList.size() )
      stop();
}

void LuaScriptEngine::stop()
{
   {
      WLock lock(mScriptListMutex);
      StlUtil::ClearContainer(mScriptList);
   }
   lua_close(mLuaState);	
}

auto_ptr<LuaScriptlet> LuaScriptEngine::create()
{
   return auto_ptr<LuaScriptlet>(new (nothrow) LuaScriptlet(this));
}

void LuaScriptEngine::linkScript(const boost::shared_ptr<LuaScriptlet>& script)
{
   WLock lock(mScriptListMutex);
   ScriptList::iterator found = find(mScriptList.begin(), mScriptList.end(), script);
   if (found == mScriptList.end())
   {
      mScriptList.push_back(script);
   }
}

void LuaScriptEngine::unlinkScript(const boost::shared_ptr<LuaScriptlet>& script)
{
   WLock lock(mScriptListMutex);
   ScriptList::iterator found = find(mScriptList.begin(), mScriptList.end(), script);
   if (found != mScriptList.end())
   {
      mScriptList.erase(found);
   }
}

void LuaScriptEngine::update(float fElapsedSec)
{
   RLock lock(mScriptListMutex);
   for (ScriptList::iterator i = mScriptList.begin();
      i != mScriptList.end(); ++i)
   {
      (*i)->update(fElapsedSec);
   }
}

// Override the LUAPanic function to provide an error message
// in such a way that our game knows how to report it
lua_CFunction LUAPanic(lua_State* L)
{
   //gLogger->Log(LOG_ERROR, "Unrecoverable LuaScriptlet Error");
   assert(false);
   return 0;
}