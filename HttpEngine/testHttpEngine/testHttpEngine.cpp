// testHttpEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <boost/make_shared.hpp>
#include <HttpEngine/IHttpEngine.hxx>

//#define _USE_BIG_FILE_TRANSFER
#define LOG_SUPPORT

#ifdef LOG_SUPPORT
#define DEFAULT_SUBSYSTEM "TestHttpEngine"
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

unsigned int REQUEST_ID_GET  = 1;
unsigned int REQUEST_ID_POST = 2;
unsigned int REQUEST_ID_POSTFORM = 3;
unsigned int REQUEST_ID_UPLOAD = 4;

#if defined(_USE_BIG_FILE_TRANSFER)
const char gTestFilePath[] = "C:\\WelbonsBootcamp\\Game\\war3\\war3.mpq";
#else
const char gTestFilePath[] = "C:\\106965642.jpg";
#endif //_USE_BIG_FILE_TRANSFER

class UploadInformation;
using namespace httpengine;
typedef boost::shared_ptr<httpengine::PostFormStreamData> PostFormDataPtr;
typedef boost::shared_ptr<UploadInformation> UploadInformationPtr;

HttpEnginePtr gEngine;
UploadInformationPtr gUploadInfoPtr;

class UploadInformation
{
public:
	UploadInformation(unsigned int requestId, const char* fileName)
		: fileSize_(0)
		, fileSeek_(0)
		, fileName_(Util::StringUtil::getNotNullString(fileName))
		, requestId_(0)
	{
		FILE* file = fopen(fileName_.c_str(), "rb");
		if (file)
		{
			fseek(file, 0, SEEK_END);
			fileSize_ = ftell(file);
			fseek(file, 0, SEEK_SET);
		}
	}

	size_t doReadFile(void* uploadBuffer, size_t bufSize)
	{
		if (fileSeek_ >= fileSize_)
		{
			return 0;
		}

		FILE* file = fopen(fileName_.c_str(), "rb");
		if (file)
		{
			if (fileSize_ > bufSize)
			{
				fseek(file, fileSeek_, SEEK_SET);

				size_t fileSurplusSize = (fileSize_ - fileSeek_);
				if (fileSurplusSize < bufSize)
				{
					bufSize = fileSurplusSize;
				}
			}
			else
			{
				bufSize = fileSize_;
			}

			fileSeek_ += fread(uploadBuffer, sizeof(char), bufSize, file);
			fclose(file);
			return bufSize;
		}
		return 0;
	}

	PostFormDataPtr getPostFormData()
	{
		if (!postFormDataPtr_)
		{
			postFormDataPtr_ 
				= boost::make_shared<httpengine::PostFormStreamData>(requestId_, gEngine.get(), (void*)NULL);
		}
		return postFormDataPtr_;
	}

	unsigned int requestId_;
	std::string fileName_;
	long long fileSeek_;
	long long fileSize_;
	PostFormDataPtr postFormDataPtr_;
};

unsigned int OnDelegateRequestReadEvent(unsigned int requestId
										, void* ptr
										, unsigned int dataSize
										, unsigned int dataCounts
										, const int errorCode
										, const int errorSubCode
										, void* userData)
{
	DebugLog( << __FUNCTION__  
		<<"| request id: " << requestId
		<<"| data: "<< (const char*)ptr
		<<"| data count: "<< dataSize
		<<"| data length: "<< dataCounts
		<<"| error code: "<< errorCode 
		<<"| sub error code: "<< errorSubCode);

	if (gUploadInfoPtr && requestId == gUploadInfoPtr->requestId_)
	{
		return gUploadInfoPtr->doReadFile(ptr, dataCounts);
	}
	return 0;
}

unsigned int OnDelegateRequestWriteEvent(unsigned int requestId
										 , void* ptr
										 , unsigned int dataSize
										 , unsigned int dataCounts
										 , const int errorCode
										 , const int errorSubCode
										 , void* userData)
{
	DebugLog( << __FUNCTION__  
		<<"| request id: " << requestId
		//<<"| data: "<< (const char*)ptr
		<<"| data count: "<< dataSize
		<<"| data length: "<< dataCounts
		<<"| error code: "<< errorCode 
		<<"| sub error code: "<< errorSubCode);

	return dataSize * dataCounts;
}

unsigned int OnDelegateRequestHeaderEvent(unsigned int requestId
										  , void* ptr
										  , unsigned int dataSize
										  , unsigned int dataCounts
										  , const int errorCode
										  , const int errorSubCode
										  , void* userData)
{
	DebugLog( << __FUNCTION__  
		<<"| request id: " << requestId
		//<<"| data: "<< (const char*)ptr
		<<"| data count: "<< dataSize
		<<"| data length: "<< dataCounts
		<<"| error code: "<< errorCode 
		<<"| sub error code: "<< errorSubCode);

	return dataSize * dataCounts;
}

void OnDelegateRequestStartedNotify(unsigned int requestId, const int errorCode, const int errorSubCode, void* userData)
{
	DebugLog( << __FUNCTION__  <<"| request id: " << requestId <<"| error code: "<< errorCode <<"| sub error code: "<< errorSubCode);

	if (requestId == REQUEST_ID_UPLOAD || requestId == REQUEST_ID_POSTFORM)
	{
		const char* filePath = gUploadInfoPtr->fileName_.c_str();
		FILE *file = fopen(filePath, "rb");
		if (file)
		{
			long fileSize = 0;
			fseek(file, 0, SEEK_END);
			fileSize = ftell(file);
			::fseek(file, 0, SEEK_SET);

			PostFormDataPtr formDataPtr = gUploadInfoPtr->getPostFormData();
			gEngine->setRequestOption(requestId, httpengine::TOT_POST_FORM
				, httpengine::HTTP_FORMTYPE_COPYNAME, "pic"
				, httpengine::HTTP_FORMTYPE_FILENAME, "123.jpg"/*gUploadInfo.fileName_.c_str()*/
				, httpengine::HTTP_FORMTYPE_STREAM, formDataPtr ? formDataPtr.get() : NULL
				, httpengine::HTTP_FORMTYPE_CONTENTSLENGTH, fileSize
				, httpengine::HTTP_FORMTYPE_CONTENTTYPE, "image/jpeg"
				, httpengine::HTTP_FORMTYPE_END);

			fclose(file);
		}
	}
	gEngine->setRequestOption(requestId, httpengine::TOT_CUSTOM_HEADER, "TestHeaderKey:", "TestHeaderValue", NULL);
}

void OnDelegateRequestErroredNotify(unsigned int requestId, const int errorCode, const int errorSubCode, void* userData)
{
	DebugLog( << __FUNCTION__  <<"| request id: " << requestId<<"| error code: "<< errorCode <<"| sub error code: "<< errorSubCode);
}

void OnDelegateRequestProgressNotify(unsigned int requestId, const double total, const double complated, const double speed)
{
	DebugLog( << __FUNCTION__  <<"| total: " << total <<"| complated: "<< complated <<"| speed: "<< speed);
}

void OnDelegateRequestComplatedNotify(unsigned int requestId, const int errorCode, const int errorSubCode, void* userData)
{
	DebugLog( << __FUNCTION__  <<"| request id: " << requestId<<"| error code: "<< errorCode <<"| sub error code: "<< errorSubCode);
}

void OnDelegateRequestStopedNotify(unsigned int requestId, const int errorCode, const int errorSubCode, void* userData)
{
	DebugLog( << __FUNCTION__  <<"| request id: " << requestId<<"| error code: "<< errorCode <<"| sub error code: "<< errorSubCode);
}

void OnDelegateRequestReleaseNotify(unsigned int requestId, const int errorCode, const int errorSubCode, void* userData)
{
	DebugLog( << __FUNCTION__  
		<<"| request id: " << requestId
		<<"| error code: "<< errorCode
		<<"| sub error code: "<< errorSubCode);
}

void OnDelegateRequestStatusNotify(unsigned int requestId, const int statusCode, void* userData)
{
	DebugLog( << __FUNCTION__  
		<<"| request id: " << requestId
		<<"| status Code: "<< statusCode);
}

void test_get(httpengine::HttpEnginePtr engine, int &outId)
{
#if defined(_USE_BIG_FILE_TRANSFER)
	outId = REQUEST_ID_GET;
	engine->startUrlRequest(REQUEST_ID_GET, "http://downloadgame.wayi.com.tw/Monkey/Monkey_1.0.9.1.exe", NULL, httpengine::HM_GET);
#else
	engine->startUrlRequest(REQUEST_ID_GET, "http://www.sina.com.cn", NULL, httpengine::HM_GET);
#endif //
}

void test_post(httpengine::HttpEnginePtr engine, int &outId)
{
	outId = REQUEST_ID_POST;
	const char *postarg = "oauth_consumer_key=140226478&oauth_nonce=cO8vk49XYTOPO8ItCW1FNvPQEbf2su&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1313256741&oauth_version=1.0&x_auth_mode=client_auth&x_auth_password=opshiwengz&x_auth_username=libo.weng%40gmail.com&oauth_signature=PcB%2FT6ec7wAH8WeN32nA0tVDhPw%3D";
	engine->startUrlRequest(REQUEST_ID_POST,"http://api.t.sina.com.cn/oauth/access_token", postarg, httpengine::HM_POST);
}

void test_postform(httpengine::HttpEnginePtr engine, int &outId)
{
	outId = REQUEST_ID_POSTFORM;
	const char* postarg = "oauth_consumer_key=1016351065&oauth_nonce=WLUwxJwAmsxcSup8Io&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1316435208&oauth_token=da141edeb8be601e053359e1b39c33cf&oauth_version=1.0&status=%B7%B6%B8%AE%B4%F3%B7%C5%B4%F3%C8%F6&oauth_signature=4wPlW6Lw9%2BzAD8aMPlHtrOvDMkw%3D";
	engine->startUrlRequest(REQUEST_ID_POSTFORM, "http://api.t.sina.com.cn/statuses/upload.json", postarg, httpengine::HM_POSTFORM);
	gUploadInfoPtr = boost::make_shared<UploadInformation>(REQUEST_ID_POSTFORM, gTestFilePath);
}

void test_upload(httpengine::HttpEnginePtr engine, int &outId)
{
	outId = REQUEST_ID_UPLOAD;
	const char* postarg = "oauth_consumer_key=140226478&oauth_nonce=aee5M8DHqz7Xc2px&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1316758831&oauth_token=bf5b4946d96d33664ea061a656c750fc&oauth_version=1.0&source=140226478&status=3123131313213&oauth_signature=qYIBaxtiaqZVwp22sn8C3Et0Grs%3D";
	engine->startUrlRequest(REQUEST_ID_UPLOAD, "http://api.t.sina.com.cn/statuses/upload.json", postarg, httpengine::HM_POSTFORM);
	gUploadInfoPtr = boost::make_shared<UploadInformation>(REQUEST_ID_UPLOAD, gTestFilePath);
}

int _tmain(int argc, _TCHAR* argv[])
{
#if defined(LOG_SUPPORT)
	Util::Log::initialize(Util::Log::File | Util::Log::VSDebugWindow, 
		Util::Log::Debug, "TestHttpEngine", L"C:\\httpengine.log");
#endif //_DEBUG_LOG

	httpengine::HttpEnginePtr enginePtr = HttpEngineFactory::createHttpEngine();

	if (!enginePtr)
	{
		return 0;
	}

	gEngine = enginePtr;
	enginePtr->initialize();

	enginePtr->OnRequestReadEvent = &OnDelegateRequestReadEvent;
	enginePtr->OnRequestWriteEvent  = &OnDelegateRequestWriteEvent;
	enginePtr->OnRequestHeaderEvent = &OnDelegateRequestHeaderEvent;
	enginePtr->OnRequestStartedNotify   = &OnDelegateRequestStartedNotify;
	enginePtr->OnRequestErroredNotify   = &OnDelegateRequestErroredNotify;
	enginePtr->OnRequestProgressNotify  = &OnDelegateRequestProgressNotify;
	enginePtr->OnRequestComplatedNotify = &OnDelegateRequestComplatedNotify;
	enginePtr->OnRequestStopedNotify    = &OnDelegateRequestStopedNotify;
	enginePtr->OnRequestReleaseNotify   = &OnDelegateRequestReleaseNotify;

	int outId = 0;
	test_get(enginePtr, outId);
	//test_post(enginePtr);
	//test_postform(enginePtr);
	//test_upload(enginePtr);

	while (1)
	{
		printf("\nPelease enter command: [quit : \"q\"]\n");
		char cc [255] = { 0 };
		gets(cc);
		if (_stricmp(cc, "q") == 0)
		{
			break;
		}
		else if (_stricmp(cc, "stop") == 0)
		{
			enginePtr->stopRequest(outId);
		}
	}
	enginePtr->unInitialize();

	// Unitialize global varibles, for memory check.
	gEngine.reset();
	enginePtr.reset();
	gUploadInfoPtr.reset();

	_CrtDumpMemoryLeaks();
	return 0;
}

