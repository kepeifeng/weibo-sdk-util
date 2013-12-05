#pragma once
#ifndef HTTPENGINE_HEDRIVER_HXX
#define HTTPENGINE_HEDRIVER_HXX


#include <map>
#include <list>
#include <boost/interprocess/detail/atomic.hpp>
#include <curl/curl.h>
#include <util/threading/Mutex.hxx>
#include <util/threading/ThreadIf.hxx>

#include "HESessionInfo.hxx"
#include "IHEDriverCommand.hxx"


namespace httpengine
{
	/**
	*  Http engine driver,this class will run work thread.
	*
	* @athor welbon
	*
	* @date 2011-04-14
	*/
	class HEDriver : public Util::ThreadIf
	{
	public:
		HEDriver(void);
		virtual ~HEDriver(void);

		void addSession(HESessionInfoPtr ptr,const unsigned int &sessionId);
		void removeSession(const unsigned int sessionId);
		void stopSession(const unsigned int sessionId);
		void clearSession();
		const HESessionInfoPtr getSession(const unsigned int sessionId);
		const unsigned int getCounts() const {
			return boost::interprocess::ipcdetail::atomic_read32(&mSessionCount);
		}

		///////////////////////////////////////////////////////////////////////
		// Util::ThreadIf interface
	private:
		virtual void thread();
		bool threadScanQueue(CURLM* multi_curl);
		void threadPerformCommand(CURLM* multi_curl);
		bool threadMultiURLRoop(CURLM* multi_curl,bool bHasHandle);
		void threadDispatchSession(CURLM* multi_curl);
		void threadQueueCleanup(CURLM* multi_curl);
		void threadFifoCleanup(void);
		void threadNotificationProgress();

	private:
		HESessionInfoPtrMap mSessionInfoPtrMap;
		mutable volatile boost::uint32_t mSessionCount;
		mutable Util::Mutex mMutexFifo;
		std::list<HEDriverCommandPtr> mCmdFifo;
	};
}

#endif //HTTPENGINE_HEDRIVER_HXX