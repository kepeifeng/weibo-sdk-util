#ifndef HTTPENGINE_HTTPCOMPAT_H
#define HTTPENGINE_HTTPCOMPAT_H

#if defined(WIN32) || defined(_WIN32) 

#   if defined(_MSC_VER)
#      pragma warning(disable: 4251)  // warning C4251:  class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 
#   endif // #if defined(_MSC_VER)

#   ifdef HTTPENGINE_EXPORTS
#      define HTTPENGINE_API __declspec(dllexport)
#	else
#      define HTTPENGINE_API __declspec(dllimport)
#   endif
#else
#   define HTTPENGINE_API 
#endif // #if defined(WIN32) || defined(_WIN32) 

#endif //HTTPENGINE_HTTPCOMPAT_H