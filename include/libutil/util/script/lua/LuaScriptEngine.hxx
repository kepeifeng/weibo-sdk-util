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

#ifndef UTIL_LUASCRIPTENGINE_HXX
#define UTIL_LUASCRIPTENGINE_HXX

#include <util/util.h>
#include <util/threading/Mutex.hxx>
#include <util/threading/RWMutex.hxx>

#include <map>
#include <list>
#include <boost/shared_ptr.hpp>

#include <lua.hpp>
#include <util/script/lua/luna.h>

namespace Util
{
   using namespace std;
   class UTIL_API LuaScriptlet;

   class UTIL_API LuaScriptEngine
   {
      //static auto_ptr<LuaScriptEngine> sScriptEngine;
      static Mutex sScriptEnginesMutex;
      typedef map<unsigned long, boost::shared_ptr<LuaScriptEngine> > ScriptEngineMap;
      static ScriptEngineMap sScriptEngines;
      lua_State* mLuaState;

      // This allows us to bind static functions to LUA
      typedef int pScriptFunction(lua_State*);
      map<string, pScriptFunction*> m_FunctionMap;

      // The list of executing scripts
      typedef list<boost::shared_ptr<LuaScriptlet> > ScriptList;
      ScriptList mScriptList;
      mutable RWMutex mScriptListMutex;

   public:
      static LuaScriptEngine& getScriptEngine();
      static void releaseScriptEngine();
      static void releaseScriptEngines();

   public: // ctor/dtor
      LuaScriptEngine();
      ~LuaScriptEngine();

   public: // methods
      lua_State* getLua() const { return mLuaState; } // returns the LUA state
      int numScripts() const; // returns how many scripts are in the list

      // Any classes that will be bound to LUA need to be registered with
      // the script engine
      template<class ClassName>
      void registerScriptClass()
      {
         Luna<ClassName>::Register(mLuaState);
      }

      // create a script
      auto_ptr<LuaScriptlet> create();

      // add script to the list
      void linkScript(const boost::shared_ptr<LuaScriptlet>& script);

      // Remove a script from the list
      void unlinkScript(const boost::shared_ptr<LuaScriptlet>& script);

      // Calls Update for every script on the list
      void update(float fElapsed);

      // This initializes the script engine, you must call this before using 
      // the 
      bool start();

   private:
      // This will destroy all scripts and close the LUA state
      void stop();
   };

} // namespace Util

#endif // #ifndef UTIL_LUASCRIPTENGINE_HXX
