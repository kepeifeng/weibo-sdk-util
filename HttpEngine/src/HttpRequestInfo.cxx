#include <util/common/StringUtil.hxx>
#include "HttpRequestInfo.hxx"

using namespace httpengine;
//------------------------------------------------------------------
RequestDetails::RequestDetails(unsigned int key
							   , const httpengine::HttpMethod method
							   , const char* url
							   , const char* postarg/* = NULL*/
							   , const char* cookie/* = NULL*/
							   , void* userData/* = NULL*/)

							   : mKey(key)
							   , mHttpMethod(method)
							   , mUrl(Util::StringUtil::getNotNullString(url))
							   , mPostArg(Util::StringUtil::getNotNullString(postarg))
							   , mCookie(Util::StringUtil::getNotNullString(cookie))
							   , mUserData(userData)
							   , mErrorCode(0)
							   , mErrorSubCode(0)
{

}

RequestDetails::~RequestDetails()
{

}
