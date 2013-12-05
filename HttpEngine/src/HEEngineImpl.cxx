#include "stdafx.h"
#include <stdarg.h>
#include "config.h"
#include "HEEngineImpl.hxx"
#include "HESessionInfo.hxx"
#include "HashHelper.hxx"

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

#define MAX_DEFAULT_REQUEST_RUNNING 10

HEEngineImpl::HEEngineImpl(void)
: mMaxRunning(MAX_DEFAULT_REQUEST_RUNNING)
{
	DebugLog(<< __FUNCTION__ << "| construct.");
}

HEEngineImpl::~HEEngineImpl(void)
{
	DebugLog(<< __FUNCTION__ << "| deconstruct.");
}

void HEEngineImpl::initialize()
{
	DebugLog(<< __FUNCTION__);

	if (!mHttpDriver)
	{
		mHttpDriver.reset(new HEDriver());
		mHttpDriver->run();
	}
}

void HEEngineImpl::unInitialize()
{
	DebugLog(<< __FUNCTION__);

	if (mHttpDriver)
	{
		mHttpDriver->shutdown();
		mHttpDriver->join();
	}
}

int HEEngineImpl::startUrlRequest(unsigned int& inOutKey
								  , const char* url
								  , const char* postarg
								  , const HttpMethod method
								  , const char* cookie/* = NULL*/
								  , void* userData /*= NULL*/)
{
	
	DebugLog(<< __FUNCTION__ 
		<< "| Url: " << Util::StringUtil::getNotNullString(url) 
		<< "| Cookie " << Util::StringUtil::getNotNullString(cookie) 
		<< "| Http method: " << method);

	int rt = HE_GENERAL_ERROR;
	if (!url)
	{
		ErrLog(<< __FUNCTION__ << "| url == NULL ");
		return rt;
	}

	if (mHttpDriver)
	{
		if (mHttpDriver->getCounts() >= mMaxRunning)
		{
			return HE_REQUEST_BEYOND_LIMITE;
		}

		if (inOutKey == 0)
		{
			inOutKey = HashHelper::UnsignedHashCode(url);
		}

		// fill details
		RequestDetailsPtr requestDetails(new RequestDetails(inOutKey, method, url, postarg, cookie, userData));

		// fill proxy
		HESessionInfoPtr pMultiSession(new HESessionInfo(requestDetails, this));
		pMultiSession->setProxy(&mProxyInfo);

		// Insert to runner session queue
		mHttpDriver->addSession(pMultiSession, inOutKey);

		// Start session
		pMultiSession->start();

		rt = HE_OK;
	}
	return rt;
}

int HEEngineImpl::stopRequest(unsigned int requestId)
{
	DebugLog(<< __FUNCTION__ << " | stop request" << " | id: " << requestId);

	if (mHttpDriver)
	{
		mHttpDriver->stopSession(requestId);
	}
	return HE_GENERAL_ERROR;
}

int HEEngineImpl::setOption(OptType optType, void* data, unsigned int dataSize, bool async)
{
	DebugLog(<< __FUNCTION__ << " | option type: " << optType);
	switch(optType)
	{
	case EOT_PROXY:
		{
			if (dataSize != sizeof(ProxyInfo))
			{
				return HE_GENERAL_ERROR;
			}
			ProxyInfo* pInfo = reinterpret_cast<ProxyInfo*>(data);

			// Do copy proxy information.
			mProxyInfo = *pInfo;
		}
		break;

	case EOT_MAXCOUNT:
		{
			if (dataSize != sizeof(int))
			{
				return HE_GENERAL_ERROR;
			}
			mMaxRunning = reinterpret_cast<long long>(data);
		}
		break;

	default:
		break;
	}
	return HE_GENERAL_ERROR;
}

void* HEEngineImpl::getOption(OptType optType, unsigned int dataSize)
{
	DebugLog(<< __FUNCTION__ << " | option type: " << optType);
	switch(optType)
	{
	case EOT_PROXY:
		{
			if (dataSize == sizeof(ProxyInfo))
			{
				return &mProxyInfo;
			}
		}
		break;

	case EOT_MAXCOUNT:
		{
            return (void*)mMaxRunning;
		}
		break;

	default:
		break;
	}
    return NULL;
}

int HEEngineImpl::setRequestOption(const int requestId, const RequestOptionType type, ...)
{
	DebugLog(<< __FUNCTION__ << "| Request option type : " << (const int)(type));

	int ret = HE_GENERAL_ERROR;

	HESessionInfoPtr sessionPtr = mHttpDriver->getSession(requestId);
	if (!sessionPtr)
	{
		ErrLog(<< __FUNCTION__ << "Request not exit!.");
		return ret;
	}

	va_list arg;
	va_start(arg, type);

	switch(type)
	{
	case TOT_POST_FORM:
		{
			sessionPtr->appendPostForm(arg);
		}
		break;

	case TOT_POST_POSTFIELDS:
		{
			const char* data = va_arg(arg , const char*);
			const int len = va_arg(arg , const int);
			sessionPtr->appendPostArg((void*)data, sizeof(char)*len);
		}
		break;

	case TOT_PROGRESS_FREQUENCE:
		{
			sessionPtr->setProgressNotifyFrequency(va_arg(arg, const int));
		}
		break;

	case TOT_CUSTOM_HEADER:
		{
			sessionPtr->appendCustomHeader(arg);
		} 
		break;

	default:
		break;
	}
	va_end(arg);
	return ret;
}

void* HEEngineImpl::getRequestOption(const int requestId, const RequestOptionType optType, unsigned int dataSize)
{
	DebugLog(<< __FUNCTION__ << "| Request option type : " << (const int)optType);

	HESessionInfoPtr sessionPtr = mHttpDriver->getSession(requestId);
	if (!sessionPtr)
	{
		ErrLog(<< __FUNCTION__ << "Request not exit!");
		return NULL;
	}

	switch(optType)
	{
	//case TOT_POST_FORM:
	//	{
	//	}
	//	break;

	//case TOT_POST_POSTFIELDS:
	//	{
	//	}
	//	break;

	//case TOT_PROGRESS_FREQUENCE:
	//	{
	//	}
	//	break;

	case TOT_TOTAL_SIZE:
		{
			return (void*)sessionPtr->getResponTotalSize();
		}
		break;

	default:
		break;
	}
	return NULL;
}

const RequestDetailsPtr HEEngineImpl::internalGetHttpRequestById(unsigned int requestId)
{
	RequestDetailsPtr pDetails;
	HESessionInfoPtr ptr = mHttpDriver->getSession(requestId);
	if( ptr)
	{
		pDetails = ptr->details();
	}
	return pDetails;
}

std::ostream& operator << (std::ostream &str, const HttpMethod& ehm)
{
	switch(ehm)
	{
	case HM_GET:
		return str << static_cast<const char*>("HM_GET");

	case HM_POST:
		return str << static_cast<const char*>("HM_POST");

	case HM_POSTFORM:
		return str << static_cast<const char*>("HM_POSTFORM");

	case HM_DELETE:
		return str << static_cast<const char*>("HM_DELETE");

	default:
		str << "HM_UNKNOWN";
		break;
	}
	return str;
}

////////////////////////////////////////////////////////////////////////////////
// Factory

HttpEnginePtr HttpEngineFactory::createHttpEngine(void)
{
	HttpEnginePtr ptr(new httpengine::HEEngineImpl());
	return ptr;
}
