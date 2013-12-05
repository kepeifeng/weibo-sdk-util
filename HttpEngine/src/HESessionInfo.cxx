#include "config.h"
#include "HESessionInfo.hxx"
#include <curl/curl.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <Urlcode.h>

#include "HEEngineImpl.hxx"


#ifdef LOG_SUPPORT
#define DEFAULT_SUBSYSTEM "HttpEngine"
#include <util/log/Logger.hxx>
#else
#define CerrLog(args_)
#define StackLog(args_)
#define DebugLog(args_)
#define InfoLog(args_)
#define WarningLog(args_)
#define ErrLog(args_)
#define CritLog(args_)
#endif

using namespace httpengine;


#ifndef _DEBUG
#define _DEBUG 1
#endif

#ifdef _DEBUG
static void dumpasiic(FILE *stream, const unsigned char *ptr, size_t size, curl_infotype infotype)
{
	unsigned int width = 0x40;
	int line = 0;
	fprintf(stream, "%d bytes (0x%x) -------------------------\n", size, size);
	for (int i = 0; i < size && line < 4; i += width, line++)
	{
		fprintf(stream, "%02x: ", i);
		for (int c = 0; (c < width) && (i + c < size); c++)
		{
			fprintf(stream, "%c", isprint(ptr[i+c]) ? ptr[i+c] : '.');
		}
		fputc('\n', stream);
	}
	fflush(stream);
}

static int my_trace(CURL *handle, curl_infotype type, unsigned char *data, size_t size, void *userp)
{
	//FILE *output = fopen("d:/curl_http.log", "a+");
	FILE* output = stdout;
	const char *text;
	static const char s_infotype[CURLINFO_END][3] = {	"* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
	fwrite(s_infotype[type], 2, 1, output);
	switch(type)
	{
	case CURLINFO_TEXT:
	case CURLINFO_HEADER_OUT:
	case CURLINFO_HEADER_IN:
		fwrite(data, size, 1, output);
		break;

	default:
		dumpasiic(output, data, size, type);
		break;
	}
	//fclose(output);
	return 0;
}
#endif

HESessionInfo::HESessionInfo(RequestDetailsPtr& pDetails,IHttpEngine* pEngine) 
: mRequestDetails(pDetails)
, mCurrentState(E_HTTPSTATUS_CLOSED)
, mEngine(pEngine)
, mCURL(NULL)
, mProgressFrequency(0)
, mLastProgressTick(0)
, mChunk(NULL)
, mFormstart(NULL)
, mFormend(NULL)
{
}

HESessionInfo::~HESessionInfo()
{
}

void HESessionInfo::start()
{
	mCurrentState = E_HTTPSTATUS_START;
}

void HESessionInfo::stop()
{
	mCurrentState = E_HTTPSTATUS_ABORT;
}

void HESessionInfo::initSessionInfo()
{
	if (!mCURL)
	{
		ErrLog(<< __FUNCTION__ << "| initialize failed, curl is null!");
		return ;
	}

	curl_easy_setopt(mCURL, CURLOPT_URL, mRequestDetails->mUrl.c_str());

	initliazeProxyInfo();
	initliazeRequestParam();

	switch(mRequestDetails->mHttpMethod)
	{
	case HM_DELETE:
		curl_easy_setopt(mCURL, CURLOPT_CUSTOMREQUEST, "DELETE");
	case HM_GET:
		initializeAsGetMethod();
		break;

	case HM_POST:
		initializeAsPostMethod();
		break;

	case HM_POSTFORM:
		initializeAsPostFormMethod();
		break;

	default:
		break;
	}

	// Head event
	curl_easy_setopt(mCURL, CURLOPT_HEADERFUNCTION, onCurlHeaderWriteFunction);
	curl_easy_setopt(mCURL, CURLOPT_HEADERDATA, this );
	curl_easy_setopt(mCURL, CURLOPT_FOLLOWLOCATION, 1L);

	// Close verify
	curl_easy_setopt(mCURL, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(mCURL, CURLOPT_SSL_VERIFYHOST, 0L);

	// Verbose
#ifdef _DEBUG
	curl_easy_setopt(mCURL, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(mCURL, CURLOPT_DEBUGFUNCTION, my_trace);
#endif

	//curl_easy_setopt(mCURL, CURLOPT_NOBODY, 1);
	//curl_easy_setopt(mCURL, CURLOPT_HEADER, 1);
	//
	//curl_easy_setopt(mCURL, CURLOPT_AUTOREFERER, 1);
	//curl_easy_setopt(mCURL, CURLOPT_NOBODY, 1);
	//curl_easy_setopt(mCURL, CURLOPT_NOPROGRESS, 1);
	//curl_easy_setopt(mCURL, CURLOPT_SSL_VERIFYPEER, FALSE);
	//curl_easy_setopt(mCURL, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)10*1024);
	//curl_easy_setopt(mCURL, CURLOPT_LOW_SPEED_LIMIT, 1);
	//curl_easy_setopt(mCURL, CURLOPT_LOW_SPEED_TIME, 300);
	//curl_easy_setopt(mCURL, CURLOPT_FAILONERROR, 1);
}

void HESessionInfo::uninitSessionInfo()
{
	if (mFormstart)
	{
		curl_formfree(mFormstart);
	}

	if (mChunk)
	{
		curl_slist_free_all(mChunk);
	}

	if (mCURL)
	{
		curl_easy_cleanup(mCURL);
		mCURL = NULL;
	}
}

void HESessionInfo::setCurrentStatus(const enHttpStatus status)
{
	mCurrentState = status; 
}

const enHttpStatus HESessionInfo::getCurrentStatus() const 
{
	return mCurrentState;
}

long HESessionInfo::getResponTotalSize()
{
	double total = 0;
	if (mRequestDetails->mHttpMethod == HM_GET)
	{
		curl_easy_getinfo(mCURL, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &total);
	}
	else
	{
		curl_easy_getinfo(mCURL, CURLINFO_CONTENT_LENGTH_UPLOAD, &total);
	}
	return (long)total;
}

void HESessionInfo::initliazeRequestParam()
{
	// set request header
	//curl_slist* pchunk = NULL;
	//buildHeaderList(true, &pchunk);
	//if( pchunk) 
	//{
	//	if(mChunk)
	//	{
	//		curl_slist_free_all(mChunk);
	//		mChunk = NULL;
	//	}
	//	curl_easy_setopt(mCURL, CURLOPT_HTTPHEADER, pchunk);
	//	mChunk = pchunk;
	//}

	// Post arg information
	appendPostArg((void*)mRequestDetails->mPostArg.c_str(), mRequestDetails->mPostArg.length());
}

void HESessionInfo::initliazeProxyInfo()
{
	if (mProxyInfo.mProxyType != ProxyInfo::EPT_NONE 
		&& !mProxyInfo.mServer.empty())
	{
		//
		switch(mProxyInfo.mProxyType)
		{
		case ProxyInfo::EPT_HTTP:
			{
				curl_easy_setopt(mCURL, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
			}
			break;
		case ProxyInfo::EPT_SOCKS4:
			{
				curl_easy_setopt(mCURL, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
			}
			break;
		case ProxyInfo::EPT_SOCKS5:
			{
				curl_easy_setopt(mCURL, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
			}
			break;
		default:
			break;
		}

		// HOST and port
		char buf[512];
		snprintf(buf, sizeof(buf) - 1, "%s:%d", mProxyInfo.mServer.c_str(), mProxyInfo.mPort);
		curl_easy_setopt(mCURL, CURLOPT_PROXY, buf);

		// User name
		if (!mProxyInfo.mUsername.empty())
		{
			curl_easy_setopt(mCURL, CURLOPT_PROXYUSERNAME, mProxyInfo.mUsername.c_str());
			curl_easy_setopt(mCURL, CURLOPT_PROXYPASSWORD, mProxyInfo.mPassword.c_str());
		}
	}
}

void HESessionInfo::initializeAsGetMethod()
{
	curl_easy_setopt(mCURL, CURLOPT_WRITEFUNCTION, onCurlWriteFunction);
	curl_easy_setopt(mCURL, CURLOPT_WRITEDATA, this);
}

void HESessionInfo::initializeAsPostMethod()
{
	curl_easy_setopt(mCURL, CURLOPT_WRITEFUNCTION, onCurlWriteFunction);
	curl_easy_setopt(mCURL, CURLOPT_WRITEDATA, this);
}

void HESessionInfo::initializeAsPostFormMethod()
{
	curl_easy_setopt(mCURL, CURLOPT_READFUNCTION, onCurlReadFunction);
//	curl_easy_setopt(mCURL, CURLOPT_READDATA, this);

	curl_easy_setopt(mCURL, CURLOPT_WRITEFUNCTION, onCurlWriteFunction);
	curl_easy_setopt(mCURL, CURLOPT_WRITEDATA, this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static function for cURL callback
//void HESessionInfo::spliteHeaderParam(char* ptr, size_t nmemb, char *outparam, char *outvalue)
//{
//	if(!ptr || *ptr == '\0')
//	{
//		return ;
//	}
//
//	const char* h = "HTTP/1.1";
//	std::string strret = ptr;
//	int iRetcode = strret.find(h);
//	if( iRetcode != std::string::npos )
//	{
//		strcpy(outparam,"retcode" );
//		strret = strret.substr( strlen(h) + 1,strret.length() - 1);
//		strret = strret.substr( 0,strret.find(" "));
//		strcpy( outvalue,strret.c_str());
//	}
//	else
//	{
//		int isplite = strret.find(":");
//		if( isplite == std::string::npos)
//		{
//			return ;
//		}
//		strcpy(outparam,strret.substr(0,isplite).c_str());
//		strcpy(outvalue,strret.substr(isplite + 2,strret.length() - 1).c_str());
//	}
//}

bool HESessionInfo::checkHttpResponse()
{
	if (mRequestDetails->mErrorCode != 0)
	{
		return false;
	}

	// If request error,notification onError.
	long lRetCode = 0;
	curl_easy_getinfo(mCURL, CURLINFO_HTTP_CODE, &lRetCode);

	if (lRetCode >= 400)
	{
		mRequestDetails->mErrorCode = HE_PROTOCOL_ERROR;
		mRequestDetails->mErrorSubCode = lRetCode;
	}
	return true;
}

size_t HESessionInfo::onCurlHeaderWriteFunction(void* ptr, size_t size, size_t nmemb, void *data)
{
	return static_cast<HESessionInfo*>(data)->doHeaderData(ptr, size, nmemb);
}

size_t HESessionInfo::onCurlWriteFunction(void* ptr, size_t size, size_t nmemb, void* data)
{
	return static_cast<HESessionInfo*>(data)->doWriteData(ptr, size, nmemb);
}

size_t Http_formget_callback(void *arg, const char *buf, size_t len)
{
	std::string* formData = reinterpret_cast<std::string*>(arg);
	if (formData)
	{
		*formData += buf;
	}
	return len;
}

size_t HESessionInfo::onCurlReadFunction(void* ptr, size_t size, size_t nmemb, void* data)
{
	PostFormStreamData* formData = static_cast<PostFormStreamData*>(data);
	if (formData)
	{
		return formData->mHttpEngine->OnRequestReadEvent(formData->mRequestId, ptr, size, nmemb,
			0, 0, formData->mUsrData);
	}
	return -1;
}

size_t HESessionInfo::doHeaderData(void* ptr, size_t size, size_t nmemb)
{
	return mEngine->OnRequestHeaderEvent(mRequestDetails->mKey, ptr, size, nmemb
		, mRequestDetails->mErrorCode, mRequestDetails->mErrorSubCode, mRequestDetails->mUserData);
}

size_t HESessionInfo::doWriteData(void* ptr, size_t size, size_t nmemb)
{
	return mEngine->OnRequestWriteEvent(mRequestDetails->mKey, ptr, size, nmemb
		, mRequestDetails->mErrorCode, mRequestDetails->mErrorSubCode, mRequestDetails->mUserData);
}

void HESessionInfo::notificationProgress(void)
{
	// Progress handle.
	if (mProgressFrequency > 0)
	{
		time_t now = time(NULL);
		if (mProgressFrequency <= (now - mLastProgressTick))
		{
			onProgress();
			mLastProgressTick = now;
		}
	}
}

//void HESessionInfo::apppendRequestHeader(const char* key,const char* value)
//{
//	if (key && value)
//	{
//		if (mRequestHMap.empty())
//		{
//			mRequestHMap.insert( make_pair(key,value) );
//		}
//		else
//		{
//			DLSessionHeaderMAP::iterator it = mRequestHMap.find(key);
//			if( it != mRequestHMap.end())
//			{
//				it->second = value;
//			}
//			else
//			{
//				mRequestHMap[key] = value;
//			}
//		}
//	}
//}

//const char* HESessionInfo::getRequestHeader(const char* key)
//{
//	if(!key || *key == '\0' || mRequestHMap.empty())
//	{
//		return NULL;
//	}
//	DLSessionHeaderMAP::iterator it = mRequestHMap.find(key);
//	if(it != mRequestHMap.end())
//	{
//		return it->second.c_str();
//	}
//	return NULL;
//}

void HESessionInfo::appendCustomHeader(va_list arg)
{
	DebugLog(<< __FUNCTION__);
	if (!arg)
	{
		DebugLog(<< __FUNCTION__ << " | Args is null!");
		return ;
	}

	int argCounter = 0;
	int arraySize  = 0;

	std::string key, value;
	while (1)
	{
		if ((argCounter % 2) == 1) //is value
		{
			value = Util::StringUtil::getNotNullString(va_arg(arg, const char*));
			key += ": ";
			key += value;
			mChunk = curl_slist_append(mChunk, key.c_str());
		}
		else // is key
		{
			key = Util::StringUtil::getNotNullString(va_arg(arg, const char*));
		}

		if (key.empty())
		{
			break;
		}
		++ argCounter;
	}

	if (mChunk)
	{
		curl_easy_setopt(mCURL, CURLOPT_HTTPHEADER, mChunk);
	}
}

void HESessionInfo::appendPostForm(va_list arg)
{
	DebugLog(<< __FUNCTION__);
	if (!arg)
	{
		DebugLog(<< __FUNCTION__ << " | Args is null!");
		return ;
	}

	curl_forms *formArray = NULL;

	int argCounter = 0;
	int arraySize  = 0;
	PostFormType argType = HTTP_FORMTYPE_UNK;//= va_arg(arg, PostFormType);
	curl_forms* arrayCursor = NULL;

	while (argType != HTTP_FORMTYPE_END)
	{
		if ((argCounter % 2) == 1 && arrayCursor) //is value
		{
			arrayCursor->value = va_arg(arg, const char*);
		}
		else // is type
		{
			argType = va_arg(arg, PostFormType);
			formArray = (curl_forms *)realloc(formArray, sizeof(curl_forms) * (arraySize + 1));
			arrayCursor = formArray + arraySize;

			arrayCursor->option = formtypeToCurlformtype(argType);
			arrayCursor->value = NULL;

			++ arraySize;
		}
		++ argCounter;
	}

	if (formArray)
	{

		CURLFORMcode code = curl_formadd(&mFormstart, &mFormend, CURLFORM_ARRAY, formArray, CURLFORM_END);
		if (code == CURL_FORMADD_OK && mFormstart)
		{
			// fill the end param.
			//if (mFormstart)
			//{
			//	curl_easy_setopt(mCURL, CURLOPT_HTTPPOST, mFormstart);
			//}
		}
		else
		{
			WarningLog(<< __FUNCTION__ << " | append form failed, result code is :" << code);
		}
		free(formArray);
	}
}

void HESessionInfo::appendPostArg(void* data, unsigned int len)
{
	const char *postarg = reinterpret_cast<const char*>(data);
	if (!postarg || *postarg == '\0')
	{
		WarningLog(<< __FUNCTION__ << "| set post arg,but post arg is NULL.");
		return ;
	}

	switch(mRequestDetails->mHttpMethod)
	{
	case HM_POST:
		{
			// Fill the post to curl.
			curl_easy_setopt(mCURL, CURLOPT_POSTFIELDS, postarg);
			curl_easy_setopt(mCURL, CURLOPT_POSTFIELDSIZE, len * sizeof(char));
		}
		break;

	case HM_POSTFORM:
		{
			const char* cursor = postarg;

			// Parsing post arg, like this : param1=value1&param2=value2&param3=value3....
			std::string argName, argValue;
			std::string *argFill = &argName;

			while (cursor && *cursor != '\0')
			{
				if (*cursor == '=')
				{
					argFill = &argValue;
				}
				else if (*cursor == '&') // param spliter
				{
					if (!argName.empty() && !argValue.empty())
					{
						curl_formadd(&mFormstart, &mFormend, CURLFORM_COPYNAME, argName.c_str(),
							CURLFORM_COPYCONTENTS, argValue.c_str(), CURLFORM_END);
					}
					argName.clear();
					argValue.clear();

					argFill = &argName;
				}
				else
				{
					if (argFill)
					{
						argFill->append(1, *cursor);
					}
				}
				++ cursor;
			}

			if (!argName.empty() && !argValue.empty())
			{
				char* valueDecode = NULL;
				curl_formadd(&mFormstart, &mFormend, CURLFORM_COPYNAME, argName.c_str(),
					CURLFORM_COPYCONTENTS, argValue.c_str(), CURLFORM_END);
			}
			else
			{
				ErrLog(<< __FUNCTION__ << "| End param , argName or argValue is NULL!");
			}
		}
		break;
	default:
		{
			WarningLog(<< __FUNCTION__ << "| Not filled post arg, may be http method wrong, http method : " << mRequestDetails->mHttpMethod);
		}
		break;
	}
}

void HESessionInfo::setProxy(ProxyInfo* pInfo)
{
	pInfo ? (mProxyInfo = *pInfo) : 0;
}

void HESessionInfo::setProgressNotifyFrequency(unsigned long seconds)
{
	mProgressFrequency = seconds;
}

RequestDetailsPtr HESessionInfo::details()
{
	return mRequestDetails;
}

//void HESessionInfo::appendResponseHeader(const char *key,const char *value)
//{
//	if( key && value)
//	{
//		if(mResponseHMap.empty())
//		{
//			mResponseHMap.insert(make_pair(key,value));
//		}
//		else
//		{
//			DLSessionHeaderMAP::iterator it = mResponseHMap.find(key);
//			if(it != mResponseHMap.end())
//			{
//				it->second = value;
//			}
//			else
//			{
//				mResponseHMap[key] = value;
//			}
//		}
//	}
//}

//const char* HESessionInfo::getResponseHeader(const char* key)
//{
//	if(!key || *key == '\0' || mResponseHMap.empty())
//	{
//		return NULL;
//	}
//	DLSessionHeaderMAP::iterator it = mResponseHMap.find(key);
//	if(it != mResponseHMap.end())
//	{
//		return it->second.c_str();
//	}
//	return NULL;
//}

//void HESessionInfo::buildHeaderList(bool bRequestHeader, curl_slist** ppchunk)
//{
//	if (!ppchunk)
//	{
//		return ;
//	}
//
//	DLSessionHeaderMAP &headermap = bRequestHeader ?  mRequestHMap : mResponseHMap;
//	if (headermap.empty())
//	{
//		return ;
//	}
//
//	DLSessionHeaderMAP::iterator it = headermap.begin();
//	while (it != headermap.end())
//	{
//		char cc[1024] = {0};
//		sprintf(cc,"%s:%s",it->first.c_str(), it->second.c_str());
//		curl_slist_append(*ppchunk, cc);
//		++ it;
//	}
//}

void HESessionInfo::onStart()
{
	mEngine->OnRequestStartedNotify(mRequestDetails->mKey, mRequestDetails->mErrorCode, mRequestDetails->mErrorSubCode, mRequestDetails->mUserData);

	if (mFormstart && HM_POSTFORM == mRequestDetails->mHttpMethod)
	{
		curl_easy_setopt(mCURL, CURLOPT_HTTPPOST, mFormstart);
	}
}

void HESessionInfo::onAbort()
{
	mEngine->OnRequestStopedNotify(mRequestDetails->mKey, mRequestDetails->mErrorCode, mRequestDetails->mErrorSubCode, mRequestDetails->mUserData);
}

void HESessionInfo::onError()
{
	mEngine->OnRequestErroredNotify(mRequestDetails->mKey, mRequestDetails->mErrorCode, mRequestDetails->mErrorSubCode, mRequestDetails->mUserData);
}

void HESessionInfo::onProgress()
{
	double total = 0;
	double complated = 0;
	double speed = 0;

	if (mRequestDetails->mHttpMethod == HM_GET)
	{
		curl_easy_getinfo(mCURL, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &total);
		curl_easy_getinfo(mCURL, CURLINFO_SIZE_DOWNLOAD, &complated);
		curl_easy_getinfo(mCURL, CURLINFO_SPEED_DOWNLOAD, &speed);
	}
	else
	{
		curl_easy_getinfo(mCURL, CURLINFO_CONTENT_LENGTH_UPLOAD, &total);
		curl_easy_getinfo(mCURL, CURLINFO_SIZE_UPLOAD, &complated);
		curl_easy_getinfo(mCURL, CURLINFO_SPEED_UPLOAD, &speed);
	}
	mEngine->OnRequestProgressNotify(mRequestDetails->mKey, total, complated, speed);
}

void HESessionInfo::onComplate()
{
	//updateDownloadDetails();
	//mCurrentState = E_HTTPSTATUS_COMPLETE;
	mEngine->OnRequestComplatedNotify(mRequestDetails->mKey, mRequestDetails->mErrorCode, mRequestDetails->mErrorSubCode, mRequestDetails->mUserData);
}

void HESessionInfo::onRelease()
{
	// Notify outside we will release this session
	mEngine->OnRequestReleaseNotify(mRequestDetails->mKey, mRequestDetails->mErrorCode, mRequestDetails->mErrorSubCode, mRequestDetails->mUserData);
}

CURLformoption HESessionInfo::formtypeToCurlformtype(const PostFormType type) const
{
	CURLformoption formid = CURLFORM_END;
	switch(type)
	{
	case HTTP_FORMTYPE_COPYNAME:
		formid = CURLFORM_COPYNAME;
		break;
	case HTTP_FORMTYPE_PTRNAME:
		formid = CURLFORM_PTRNAME;
		break;
	case HTTP_FORMTYPE_NAMELENGTH:
		formid = CURLFORM_NAMELENGTH;
		break;
	case HTTP_FORMTYPE_COPYCONTENTS:
		formid = CURLFORM_COPYCONTENTS;
		break;
	case HTTP_FORMTYPE_PTRCONTENTS:
		formid = CURLFORM_PTRCONTENTS;
		break;
	case HTTP_FORMTYPE_CONTENTSLENGTH:
		formid = CURLFORM_CONTENTSLENGTH;
		break;
	case HTTP_FORMTYPE_FILECONTENT:
		formid = CURLFORM_FILECONTENT;
		break;
	case HTTP_FORMTYPE_ARRAY:
		formid = CURLFORM_ARRAY;
		break;
	case HTTP_FORMTYPE_OBSOLETE:
		formid = CURLFORM_OBSOLETE;
		break;
	case HTTP_FORMTYPE_FILE:
		formid = CURLFORM_FILE;
		break;
	case HTTP_FORMTYPE_BUFFER:
		formid = CURLFORM_BUFFER;
		break;
	case HTTP_FORMTYPE_BUFFERPTR:
		formid = CURLFORM_BUFFERPTR;
		break;
	case HTTP_FORMTYPE_BUFFERLENGTH:
		formid = CURLFORM_BUFFERLENGTH;
		break;
	case HTTP_FORMTYPE_CONTENTTYPE:
		formid = CURLFORM_CONTENTTYPE;
		break;
	case HTTP_FORMTYPE_CONTENTHEADER:
		formid = CURLFORM_CONTENTHEADER;
		break;
	case HTTP_FORMTYPE_FILENAME:
		formid = CURLFORM_FILENAME;
		break;
	case HTTP_FORMTYPE_OBSOLETE2:
		formid = CURLFORM_OBSOLETE2;
		break;
	case HTTP_FORMTYPE_STREAM:
		formid = CURLFORM_STREAM;
		break;
	default:
		break;
	}
	return formid;
}
