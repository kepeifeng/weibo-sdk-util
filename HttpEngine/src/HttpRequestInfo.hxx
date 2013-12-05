#ifndef HTTPENGINE_INTERFACE_REQUESTINFO_HXX
#define HTTPENGINE_INTERFACE_REQUESTINFO_HXX

#include <string>
#include <boost/shared_ptr.hpp>
#include "HttpCompat.hxx"
#include "IHttpEngineCommon.hxx"


namespace httpengine
{
	struct HTTPENGINE_API RequestDetails
	{
		RequestDetails(unsigned int key
			, const httpengine::HttpMethod method
			, const char* url
			, const char* postarg = NULL
			, const char* cookie = NULL
			, void* userData = NULL);

		virtual ~RequestDetails();

		unsigned int mKey;
		volatile int mErrorCode;
		volatile int mErrorSubCode;
		void* mUserData;

		std::string mPostArg;
		std::string mUrl;
		std::string mCookie;

		HttpMethod mHttpMethod;
	};

	typedef boost::shared_ptr<RequestDetails> RequestDetailsPtr;
}
#endif //#ifndef HTTPENGINE_INTERFACE_REQUESTINFO_HXX
