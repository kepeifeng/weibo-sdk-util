#include "stdafx.h"
#include "HashHelper.hxx"
#include <cstring>
#include <wchar.h>

long HashHelper::HashCode(const char* str)
{
   long hash = 0;
   if (str)
   {
      size_t strLength = ::strlen(str);
      for (size_t i = 0; i < strLength; ++i)
      {
         hash = 31 * hash + str[i];
      }
   }
   return hash;
}

//static
long HashHelper::HashCode(const wchar_t* str)
{
   long hash = 0;
   if (str)
   {
      size_t strLength = ::wcslen(str);
      for (size_t i = 0; i < strLength; ++i)
      {
         hash = 31 * hash + str[i];
      }
   }
   return hash;
}

unsigned long HashHelper::UnsignedHashCode(const char* str)
{
   unsigned long hash = 0;
   if (str)
   {
      size_t strLength = ::strlen(str);
      for (size_t i = 0; i < strLength; ++i)
      {
         hash = 31 * hash + str[i];
      }
   }
   return hash;
}

unsigned long HashHelper::UnsignedHashCode(const wchar_t* str)
{
   unsigned long hash = 0;
   if (str)
   {
      size_t strLength = ::wcslen(str);
      for (size_t i = 0; i < strLength; ++i)
      {
         hash = 31 * hash + str[i];
      }
   }
   return hash;
}
