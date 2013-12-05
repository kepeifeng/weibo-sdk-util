#ifndef UTIL_STRINGUTIL_HXX
#define UTIL_STRINGUTIL_HXX

#include <util/util.h>
#include <util/compat.hxx>

#include <string>
#include <list>
#include <deque>
#include <vector>
using namespace std;

namespace Util
{
   class UTIL_API StringUtil
   {
   public:
      static const string sEmpty;
#if defined(_WIN32) || defined(WIN32)
      static const std::wstring sWEmpty;
#endif

      static const char* getNotNullString(const char* strValue)
      {
         return (strValue?strValue:sEmpty.c_str());
      }


      template<typename StringPtrType>
      static bool NullOrEmpty(StringPtrType strPtr)
      {
         return !(strPtr && strPtr[0]);
      }

      template<typename StringPtrType>
      static bool NotNullOrEmpty(StringPtrType strPtr)
      {
         return (strPtr && strPtr[0]);
      }

      template<typename StringType>
      static void Trim(StringType& toTrim)
      {
         TrimRight(toTrim);
         TrimLeft(toTrim);
      }

      template<typename StringType>
      static void TrimLeft(StringType& toTrim)
      {
         while (::isspace(toTrim[0]))
            toTrim.erase(toTrim.begin(), toTrim.begin() + 1);
      }

      template<typename StringType>
      static void TrimRight(StringType& toTrim)
      {
         while (::isspace(toTrim[toTrim.length() - 1]))
            toTrim.erase(toTrim.length() - 1);
      }

      template<typename StringType>
      static void replace(StringType& strBig, const StringType& strSrc, const StringType& strDst)
      {
         typedef typename StringType::size_type size_type;
         size_type pos = 0;
         size_type srcLen = strSrc.size();
         size_type dstLen = strDst.size();

         while((pos=strBig.find(strSrc, pos)) != StringType::npos)
         {
            strBig.replace(pos, srcLen, strDst);
            pos += dstLen;
         }
      }
   };
} // namespace Util

#endif // #ifndef UTIL_STRINGUTIL_HXX