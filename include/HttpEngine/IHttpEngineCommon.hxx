#ifndef HTTPENGINE_IHTTPENGINECOMMON_HXX
#define HTTPENGINE_IHTTPENGINECOMMON_HXX

#include <string>
#include <util/delegate/delegate.hxx>
#include <util/common/StringUtil.hxx>

namespace httpengine
{
	enum HttpMethod
	{
		HM_GET,
		HM_POST,
		HM_POSTFORM,
		HM_DELETE
	};

	/** from wininet important err code */
	enum HttpEngineErrorCode 
	{
		HE_OK = 0,
		HE_GENERAL_ERROR = -1,
		HE_REQUEST_BEYOND_LIMITE = -2,

		HE_PROTOCOL_ERROR   = -14,
		HE_CURL_ERROR		= -15,
	};

	enum OptType
	{
		EOT_UNKKNOW = 0,
		EOT_PROXY,  // struct ProxyInfo
		EOT_MAXCOUNT, // int count
	};

	/** Request option type */
	enum RequestOptionType
	{
		TOT_POST_FORM,       // Reference PostFormType
		TOT_POST_POSTFIELDS, // Fields
		TOT_PROGRESS_FREQUENCE, // int , for progressing notify.
		TOT_TOTAL_SIZE, // Total size, called by getRequestOptionType at complated notify.
		TOT_CUSTOM_HEADER, // Custom request header.
	};

	/** Http form, for post method,use setRequestOption. */
	enum PostFormType
	{
		HTTP_FORMTYPE_UNK,
		HTTP_FORMTYPE_COPYNAME,
		HTTP_FORMTYPE_PTRNAME,
		HTTP_FORMTYPE_NAMELENGTH,
		HTTP_FORMTYPE_COPYCONTENTS,
		HTTP_FORMTYPE_PTRCONTENTS,
		HTTP_FORMTYPE_CONTENTSLENGTH,
		HTTP_FORMTYPE_FILECONTENT,
		HTTP_FORMTYPE_ARRAY,
		HTTP_FORMTYPE_OBSOLETE,
		HTTP_FORMTYPE_FILE,
		HTTP_FORMTYPE_BUFFER,
		HTTP_FORMTYPE_BUFFERPTR,
		HTTP_FORMTYPE_BUFFERLENGTH,
		HTTP_FORMTYPE_CONTENTTYPE,
		HTTP_FORMTYPE_CONTENTHEADER,
		HTTP_FORMTYPE_FILENAME,
		HTTP_FORMTYPE_END,
		HTTP_FORMTYPE_OBSOLETE2,
		HTTP_FORMTYPE_STREAM, // followed by PostFormStreamData object.
		HTTP_FORMTYPE_LASTENTRY
	};

	struct ProxyInfo
	{
		enum eProxyType
		{
			EPT_NONE,
			EPT_HTTP,
			EPT_SOCKS4,
			EPT_SOCKS5,
		};
		ProxyInfo(const char* server = NULL,
			unsigned int port = 8080,
			const char* username = NULL,
			const char* password = NULL)

			: mProxyType(EPT_NONE)
			, mServer(Util::StringUtil::getNotNullString(server))
			, mPort(port)
			, mUsername(Util::StringUtil::getNotNullString(username))
			, mPassword(Util::StringUtil::getNotNullString(password))
		{
		}

		ProxyInfo &operator = (ProxyInfo &proxy)
		{
			mProxyType = proxy.mProxyType;
			mServer = proxy.mServer;
			mPort = proxy.mPort;
			mUsername = proxy.mUsername;
			mPassword = proxy.mPassword;
			return *this;
		}
		eProxyType mProxyType;
		std::string mServer;
		unsigned int mPort;

		std::string mUsername;
		std::string mPassword;
	};

	class IHttpEngine;
	
	// For file upload
	struct PostFormStreamData
	{
		PostFormStreamData(unsigned int requestId, IHttpEngine *engine, void* usrData)
			: mRequestId(requestId)
			, mHttpEngine(engine)
			, mUsrData(usrData)
		{

		}
		unsigned int mRequestId;
		IHttpEngine *mHttpEngine;
		void *mUsrData;	
	};
}

#endif //HTTPENGINE_IHTTPENGINECOMMON_HXX