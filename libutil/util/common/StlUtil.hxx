#ifndef UTIL_STLUTIL_HXX
#define UTIL_STLUTIL_HXX

#include <util/util.h>
#include <util/common/StringUtil.hxx>
#include <set>
#include <map>
using namespace std;

namespace Util
{
   class UTIL_API StlUtil
   {
   public:
      template<typename T>
      static void ClearContainer(T& container)
      {
         T().swap(container);
      }

      template<class ContainerType>
      static typename ContainerType::iterator Erase(ContainerType& container, typename ContainerType::iterator& eraseIterator)
      {
         if (eraseIterator != container.end())
         {
            eraseIterator = container.erase(eraseIterator);
         }
         return eraseIterator;
      }

      template<class ContainerType>
      static typename ContainerType::iterator TreeErase(ContainerType& container, typename ContainerType::iterator& eraseIterator)
      {
         if (eraseIterator != container.end())
         {
            typename ContainerType::iterator toDelete = eraseIterator;
            ++eraseIterator;
            container.erase(toDelete);
         }
         return eraseIterator;
      }
      template<class _Kty, class _Pr, class _Alloc>
      static typename std::set<_Kty, _Pr, _Alloc>::iterator Erase(std::set<_Kty, _Pr, _Alloc>& container, typename std::set<_Kty, _Pr, _Alloc>::iterator& eraseIterator)
      {
         return TreeErase(container, eraseIterator);
      }

      template<class _Kty, class _Ty, class _Pr, class _Alloc>
      static typename std::map<_Kty, _Ty, _Pr, _Alloc>::iterator Erase(std::map<_Kty, _Ty, _Pr, _Alloc>& container, typename std::map<_Kty, _Ty, _Pr, _Alloc>::iterator& eraseIterator)
      {
         return TreeErase(container, eraseIterator);
      }
   };
} // namespace Util

#endif // #ifndef UTIL_STLUTIL_HXX
