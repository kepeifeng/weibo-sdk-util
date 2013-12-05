#pragma once
#ifndef HTTPENGINE_HEDRIVER_COMMAND_HXX
#define HTTPENGINE_HEDRIVER_COMMAND_HXX

#include "IHEDriverCommand.hxx"
#include "HESessionInfo.hxx"

namespace httpengine
{
	class HEDriverCommandBase : public IHEDriverCommand
	{
	public:
		HEDriverCommandBase();
	  ~HEDriverCommandBase();

		void execute(HESessionInfoPtrMap& sessionMap, void* param);
	};

	class HEDriverCommandAdd : public HEDriverCommandBase
	{
	public:
		HEDriverCommandAdd(HESessionInfoPtr ptrSession);
		~HEDriverCommandAdd();

		void execute(HESessionInfoPtrMap& sessionMap, void* param);

	protected:
		HESessionInfoPtr mHESessionInfoPtr;
	};

	class HEDriverCommandRemove : public HEDriverCommandBase
	{
	public:
		HEDriverCommandRemove(int iKey);
		~HEDriverCommandRemove();

		void execute(HESessionInfoPtrMap& sessionMap, void* param);

	protected:
		int mDelKey;
	};

	class HEDriverCommandStop : public HEDriverCommandBase
	{
	public:
		HEDriverCommandStop(int iKey);
		~HEDriverCommandStop();

		void execute(HESessionInfoPtrMap& sessionMap, void* param);

	protected:
		int mKey;
	};

	class HEDriverCommandClear : public HEDriverCommandBase
	{
	public:
		HEDriverCommandClear();
		~HEDriverCommandClear();

		void execute(HESessionInfoPtrMap& sessionMap, void* param);
	};
}

#endif //__DL_MULTI_RUNNER_CMD_H__