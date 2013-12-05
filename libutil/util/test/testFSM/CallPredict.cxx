#include "stdafx.h"

#include "CallPredict.hxx"
#include "CallEvent.hxx"

#include <util/fsm/Machine.hxx>

using namespace Util;
using namespace Testor;

CallPredicate::CallPredicate(CallEvent::Type type, Target match)
: mType(type),
mMatch(match)
{}

bool
CallPredicate::operator()(const Event* event,
						  const MachineBase& machine)
{
	const CallEvent* tevent = dynamic_cast<const CallEvent*>(event);
	if (!tevent)
	{
		return false;
	}
	else if (tevent->getType() != mType)
	{
		return false;
	}
	return true;
}
