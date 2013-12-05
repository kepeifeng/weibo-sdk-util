#pragma once
#ifndef HTTPENGINE_HESESSIONINFO_HXX
#define HTTPENGINE_HESESSIONINFO_HXX

#include <map>
#include <curl/curl.h>
#include "HttpComDef.hxx"
#include "IHttpEngine.hxx"

namespace httpengine
{
	enum enHttpStatus
	{
		E_HTTPSTATUS_CLOSED = 0, 
		E_HTTPSTATUS_START,
		E_HTTPSTATUS_ABORT,
		E_HTTPSTATUS_WAITCOMPLATE,
		E_HTTPSTATUS_COMPLETE,
		E_HTTPSTATUS_ERROR,
		
	};

	/** Download information 
	* 
	* @author welbon
	*
	* @date 2011-04-14
	*/
	class HESessionInfo
	{
	public:
		HESessionInfo(RequestDetailsPtr& pDetails, IHttpEngine* pEngine);
		virtual ~HESessionInfo();

		void start();
		void stop();

		//void apppendRequestHeader(const char* key,const char* value);
		//const char* getRequestHeader(const char* key);

		void appendCustomHeader(va_list arg);
		void appendPostForm(va_list arg);
		void appendPostArg(void* data, unsigned int len);

		/** Spcify upload file size*/
		void setUploadFileSize(const long fileSize);

		/** Proxy information */
		void setProxy(ProxyInfo* pInfo);

		/** This method tells drivers need to notice about 
		the frequency of 'seconds' seconds of the caller*/
		void setProgressNotifyFrequency(unsigned long seconds);

		/** Request details, only one at each session info,
		storage reqpuest response code and post information */
		RequestDetailsPtr details();

		/** Current state, used by HttpEngine Driver */
		void setCurrentStatus(const enHttpStatus status);
		const enHttpStatus getCurrentStatus() const;

		/** Total size */
		long getResponTotalSize();

		/////////////////////////////////////////////////////////////////
		// Work thread called.

		/** Please called this function after all option
		* it's like this :
		* setRequestMethod(XXMETHOD)
		* //TODO any other option.
		* initSessionInfo();
		* ..
		* This function must called by work thread.
		*/
		void initSessionInfo();
		void uninitSessionInfo();

		void onStart();
		void onAbort();
		void onError();
		void onProgress();
		void onComplate();
		void onRelease();

		void notificationProgress(void);

		CURL* mCURL;

	protected:
		/** Initialize common information */
		void initliazeRequestParam();
		void initliazeProxyInfo();

		/* Initialize method information */
		void initializeAsGetMethod();
		void initializeAsPostMethod();
		void initializeAsPostFormMethod();

		/** header function */
		void appendResponseHeader(const char* key,const char* value);
		const char* getResponseHeader(const char* key);
		void buildHeaderList(bool bRequestHeader,curl_slist** ppchunk);

		////////////////////////////////////////////////////
		// internal options
		bool checkHttpResponse(void);
		CURLformoption formtypeToCurlformtype(const PostFormType type) const;

	private:
		////////////////////////////////////////////////////
		// data callback
		static size_t onCurlHeaderWriteFunction(void* ptr, size_t size, size_t nmemb, void* data);
		static size_t onCurlWriteFunction(void* ptr, size_t size, size_t nmemb, void* data);
		static size_t onCurlReadFunction(void* ptr, size_t size, size_t nmemb, void* data);

		size_t doHeaderData(void* ptr, size_t size, size_t nmemb);
		size_t doWriteData(void* ptr, size_t size, size_t nmemb);

		static void updateProgress(HESessionInfo *pDLMultiInfo,size_t realsize);

		/** Splite the header response information */
		static void spliteHeaderParam(char* ptr, size_t nmemb, char* outparam, char* outvalue);

	private:
		//typedef boost::shared_ptr<RequestDetails> URLRequestDetailsPtr;
		//typedef std::map<std::string ,std::string> DLSessionHeaderMAP ;

		enHttpStatus mCurrentState ;///< Reference enHttpStatus
		RequestDetailsPtr mRequestDetails;
		IHttpEngine* mEngine;

		/** Headers map*/
		//DLSessionHeaderMAP mRequestHMap;
		//DLSessionHeaderMAP mResponseHMap;
		curl_slist* mChunk ;

		/** post form */
		curl_httppost* mFormstart;
		curl_httppost* mFormend;

		/** Proxy information */
		ProxyInfo mProxyInfo;

		/** call "setProgressNotifyFrequency" function to set this value.
		* if this value will not 0, duplicate notifications outside */
		unsigned long mProgressFrequency;
		time_t mLastProgressTick;

		//friend class HEDriver;
		//friend class HEDriverCommandRemove;
	};

	typedef boost::shared_ptr<HESessionInfo> HESessionInfoPtr;
}

#endif //HTTPENGINE_HESESSIONINFO_HXX