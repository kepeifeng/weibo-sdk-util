#ifndef UTIL_H
#define UTIL_H

#if defined(WIN32) || defined(_WIN32) 
#   ifdef UTIL_EXPORTS
#      define UTIL_API __declspec(dllexport)
#   elif defined(USE_UTIL_DLL)
#      define UTIL_API __declspec(dllimport)
#   else
#      define UTIL_API 
#   endif
#else
#   define UTIL_API 
#endif // #if defined(WIN32) || defined(_WIN32) 

#endif //UTIL_H
