#ifndef UTIL_COMPAT_HXX
#define UTIL_COMPAT_HXX 

#if defined(__INTEL_COMPILER ) && defined( __OPTIMIZE__ )
#undef __OPTIMIZE__ // wierd intel bug with ntohs and htons macros
#endif

#if defined(HAVE_SYS_INT_TYPES_H)
#include <sys/int_types.h>
#endif

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif
 
#ifndef _WIN32
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#ifdef _WIN32
//# include <winsock2.h>
# include <windows.h>
# include <winbase.h>
# ifndef _WIN32_WCE
# include <errno.h>
# include <io.h>
# endif

#ifndef __BIT_TYPES_DEFINED__ /* sleepcat DB uses this */ 
typedef unsigned long int u_int32_t;
typedef long int ssize_t;
#endif // #ifdef _WIN32

#endif // #ifdef _WIN32

#if TARGET_OS_MAC /* TARGET_OS_MAC #defined in OS X SDK, "TargetConditionals.h" */
# include <netdb.h>
# include <arpa/nameser_compat.h>
# ifdef __MWERKS__ /* this is a <limits.h> bug filed with Apple, Radar# 3657629. */
#  ifndef __SCHAR_MAX__ 
#   define __SCHAR_MAX__ 127
#  endif // #  ifndef __SCHAR_MAX__ 
# endif // # ifdef __MWERKS__ 
#endif // #if defined(TARGET_OS_MAC)

#if defined(__SUNPRO_CC)
# if defined(_TIME_T)
 using std::time_t;
# endif // # if defined(_TIME_T)
#include <time.h>
#include <memory.h>
#include <string.h>
#endif // #if defined(__SUNPRO_CC)

#include <cstring>

#if defined(_WIN32) || defined(__QNX__)
# ifdef _WIN32_WCE
#  define strcasecmp(a,b) _stricmp(a,b)
#  define strncasecmp(a,b,c) _strnicmp(a,b,c)
# else
#  define strcasecmp(a,b) stricmp(a,b)
#  define strncasecmp(a,b,c) strnicmp(a,b,c)
# endif // # ifdef _WIN32_WCE
#endif

#ifndef _WIN32
#include <pthread.h>
#endif

#if defined(__QNX__) || defined(__sun)
typedef unsigned int u_int32_t;
#endif

#if !defined(T_NAPTR)
#define T_NAPTR 35
#endif

#if !defined(T_SRV)
#define T_SRV 33
#endif

#if !defined(T_AAAA)
#define T_AAAA 28
#endif

namespace Util
{

template<typename _Tp>
inline const _Tp&
resipMin(const _Tp& __a, const _Tp& __b)
{
   if (__b < __a) return __b; return __a;
}

template<typename _Tp>
inline const _Tp&
resipMax(const _Tp& __a, const _Tp& __b) 
{
   if (__a < __b) return __b; return __a;
}

#if defined(__DARWIN__)
typedef size_t socklen_t;
#endif
}

#if defined( _WIN32 )
typedef __int64          Int64;
typedef unsigned __int64 UInt64;
#else
typedef long long        Int64;
typedef unsigned long long UInt64;
#endif


//template "levels; ie REASONABLE and COMPLETE
//reasonable allows most things such as partial template specialization,
//etc...like most compilers and VC++2003+.
//COMPLETE would allow template metaprogramming, template< template< > > tricks,
//etc...REASONABLE should always be defined when COMPLETE is defined.


#if defined(_MSC_VER) && (MSC_VER >= 1200)
#define REASONABLE_TEMPLATES
#endif
#if !defined(__SUNPRO_CC) && !defined(__INTEL_COMPILER)
#define REASONABLE_TEMPLATES
#endif

#if defined( _WIN32 )
#  define snprintf sprintf_s
#  define atoll _atoi64
#  define fseeko _fseeki64
#  define ftello _ftelli64
#endif

#if !defined( _WIN32 )
# if _HAS_EXCEPTIONS
 #define _TRY_BEGIN	try {
 #define _CATCH(x)	} catch (x) {
 #define _CATCH_ALL	} catch (...) {
 #define _CATCH_END	}

 #define _RAISE(x)	throw (x)
 #define _RERAISE	throw

 #define _THROW0()	throw ()
 #define _THROW1(x)	throw (...)

 #define _THROW(x, y)	throw x(y)
 #define _THROW_NCEE(x, y)	_THROW(x, y)

# else /* _HAS_EXCEPTIONS */
 #define _TRY_BEGIN	{{
 #define _CATCH(x)	} if (0) {
 #define _CATCH_ALL	} if (0) {
 #define _CATCH_END	}}

 #define _RAISE(x)	::std:: _Throw(x)

 #define _RERAISE

 #define _THROW0()
 #define _THROW1(x)
 #define _THROW(x, y)	x(y)._Raise()
 #define _THROW_NCEE(x, y)	_THROW(x, y)
#endif /* _HAS_EXCEPTIONS */

#endif

#if defined( _WIN32 )
# define bcopy(src, dest, count) memcpy(dest, src, count)
#endif

#endif // #ifndef TUTIL_COMPAT_HXX

