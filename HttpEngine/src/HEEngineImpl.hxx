#pragma once
#ifndef HTTPENGINE_HEENGINEIMPL_HXX
#define HTTPENGINE_HEENGINEIMPL_HXX

#include <curl/curl.h>
#include "IHttpEngine.hxx"
#include "HEDriver.hxx"

namespace httpengine
{
	/**
	* HTTP engine for multi curl.
	*
	* @author welbon
	*
	* @date 2011-04-12
	*/
	class HEEngineImpl : public IHttpEngine
	{
	public:
		HEEngineImpl();
		virtual ~HEEngineImpl();

		///////////////////////////////////////////////////////////////////////
		//  IHttpEngine interface
	public:
		virtual void initialize();
		virtual void unInitialize();

		virtual int startUrlRequest(
			unsigned int& inOutKey
			, const char* url
			, const char* postarg
			, const HttpMethod method
			, const char* cookie = NULL
			, void* userData = NULL);

		virtual int stopRequest(unsigned int requestId);
		virtual int setOption(OptType optType, void* data, unsigned int dataSize, bool async);
		virtual void* getOption(OptType optType, unsigned int dataSize);
		virtual int setRequestOption(const int requestId, const RequestOptionType type, ...);
		virtual void* getRequestOption(const int requestId, const RequestOptionType optType, unsigned int dataSize);

		virtual const RequestDetailsPtr internalGetHttpRequestById(unsigned int requestId);

	private:
		long long mMaxRunning;
		boost::shared_ptr<HEDriver> mHttpDriver;
		ProxyInfo mProxyInfo;
	};
}

#endif //HTTPENGINE_MULTIHTTPENGINE_HXX