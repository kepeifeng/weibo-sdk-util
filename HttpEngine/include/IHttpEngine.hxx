#ifndef HTTPENGINE_HTTPENGINE_HXX
#define HTTPENGINE_HTTPENGINE_HXX

#include <boost/shared_ptr.hpp>
#include "HttpCompat.hxx"
#include "IHttpEngineCommon.hxx"


/** HTTP Engine interface
* 
* @author welbon
*
* @date 2011-08-08
*/
namespace httpengine
{
	class IHttpEngine;
	typedef boost::shared_ptr<IHttpEngine> HttpEnginePtr;

	class HTTPENGINE_API HttpEngineFactory
	{
	public:
		static HttpEnginePtr createHttpEngine(void);
	};

	class HTTPENGINE_API IHttpEngine
	{
	public:
		virtual ~IHttpEngine(){}

		virtual void initialize() = 0;
		virtual void unInitialize() = 0;

		virtual int startUrlRequest(
			unsigned int& requestId
			, const char* url
			, const char* postarg
			, const HttpMethod method
			, const char* cookie = NULL
			, void* userData = NULL ) = 0;

		virtual int stopRequest(unsigned int requestId) = 0;
		virtual int setOption(OptType optType, void* data, unsigned int dataSize, bool async) = 0;
		virtual void* getOption(OptType optType, unsigned int dataSize) = 0;
		virtual int setRequestOption(const int requestId, const RequestOptionType type, ...) = 0;
		virtual void* getRequestOption(const int requestId, const RequestOptionType optType, unsigned int dataSize) = 0;

	public:
		/** requestId, data, dataSize, dataCounts, errorCode, subErrorCode, userData */
		typedef Util::Delegate<unsigned int (unsigned int, void*, unsigned int, unsigned int, const int, const int, void*)
			, Util::PolicyMultiThreaded> HttpEngineActionEvent;

		/** requestId, errorCode, errorSubCode, userData */
		typedef Util::Delegate<void (unsigned int, const int, const int, void*)
			, Util::PolicyMultiThreaded> HttpEngineNotify;

		/** requestId, total, complated, speed */
		typedef Util::Delegate<void (unsigned int, const double, const double, const double)
			, Util::PolicyMultiThreaded> HttpEngineProgressNotify;

		HttpEngineActionEvent OnRequestReadEvent;
		HttpEngineActionEvent OnRequestWriteEvent;
		HttpEngineActionEvent OnRequestHeaderEvent;

		HttpEngineNotify OnRequestStartedNotify;// called by enqueue, but not start connect
		HttpEngineNotify OnRequestErroredNotify;// request occured error.
		HttpEngineNotify OnRequestComplatedNotify;// request complated
		HttpEngineNotify OnRequestStopedNotify;// request abort.
		HttpEngineNotify OnRequestReleaseNotify;// request released.

		HttpEngineProgressNotify OnRequestProgressNotify;// processing
	};
}
#endif //#ifndef HTTPENGINE_INTERFACE_HTTPENGINE_HXX
