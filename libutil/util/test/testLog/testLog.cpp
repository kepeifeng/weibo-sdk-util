// testLog.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <util/log/Logger.hxx>
#define DEFAULT_SUBSYSTEM "Testor"

void testStackLog()
{
	Util::Log::initialize(Util::Log::Cout|Util::Log::File, Util::Log::Stack, "testor", L"stack.log");
	StackLog( << __FUNCTION__);
	DebugLog( << __FUNCTION__);
	InfoLog( << __FUNCTION__);
	ErrLog( << __FUNCTION__);
}
void testDebugLog()
{
	Util::Log::initialize(Util::Log::Cout|Util::Log::File, Util::Log::Debug, "testor", L"debug.log");
	StackLog( << __FUNCTION__);
	DebugLog( << __FUNCTION__);
	InfoLog( << __FUNCTION__);
	ErrLog( << __FUNCTION__);
}
void testInfoLog()
{
	Util::Log::initialize(Util::Log::Cout|Util::Log::File, Util::Log::Info, "testor", L"info.log");
	StackLog( << __FUNCTION__);
	DebugLog( << __FUNCTION__);
	InfoLog( << __FUNCTION__);
	ErrLog( << __FUNCTION__);
}
void testErrLog()
{
	Util::Log::initialize(Util::Log::Cout|Util::Log::File, Util::Log::Err, "testor", L"err.log");
	StackLog( << __FUNCTION__);
	DebugLog( << __FUNCTION__);
	InfoLog( << __FUNCTION__);
	ErrLog( << __FUNCTION__);
}
int _tmain(int argc, _TCHAR* argv[])
{
	testStackLog();
	testDebugLog();
	testInfoLog();
	testErrLog();
	_gettchar();
	return 0;
}



