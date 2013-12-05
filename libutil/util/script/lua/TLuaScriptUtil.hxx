#ifndef UTIL_TLUASCRIPTUTIL_HXX
#define UTIL_TLUASCRIPTUTIL_HXX

#include <util/util.h>
#include <memory>

namespace Util
{
   class LuaScriptlet;

   class UTIL_API TLuaScriptUtil
   {
   private:
      TLuaScriptUtil()  // hide construtor
      { /*Empty*/}

   public:
      ~TLuaScriptUtil()
      { /*Empty*/}

      static int runLuaScript(Util::LuaScriptlet* script, const char* fileName, int resId, const char* libFileName);


   };
} // namespace Util

#endif // #ifndef UTIL_TLUASCRIPTUTIL_HXX