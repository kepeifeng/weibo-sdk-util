#ifndef HASHHELPER_HXX
#define HASHHELPER_HXX

class HashHelper
{
public:
	static long HashCode(const char* str);
	static long HashCode(const wchar_t* bstrStr);
	static unsigned long UnsignedHashCode(const char* str);
	static unsigned long UnsignedHashCode(const wchar_t* str);
};

#endif // #ifndef HASHHELPER_HXX

