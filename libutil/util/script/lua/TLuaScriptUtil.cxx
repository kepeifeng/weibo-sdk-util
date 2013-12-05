#include "stdafx.h"

#include <util/script/lua/TLuaScriptUtil.hxx>
#include <util/script/lua/LuaScriptlet.hxx>
#include <util/common/StlUtil.hxx>

using namespace Util;

int TLuaScriptUtil::runLuaScript(Util::LuaScriptlet* script, const char* fileName, int resId, const char* libFileName)
{
   int bSucceed = -1;
   if(script)
   {
      if (Util::StringUtil::NotNullOrEmpty(fileName))
      {
         bSucceed =  script->runFile(fileName);
      }
      if (bSucceed !=  Util::LuaScriptlet::lsc_NoError)
      {
# ifdef _DEBUG
         return script->runResource(resId, libFileName, true);
#else
         return script->runResource(resId, libFileName, false);
#endif
      }
   }
   return bSucceed;
}