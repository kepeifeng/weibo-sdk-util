#include "stdafx.h"

#include "CallEvent.hxx"
#include "CallMachine.hxx"

using namespace Testor;

CallEvent::CallEvent(Type type,
					 CallMachine& imMachine)
					 :Event(&imMachine)
					 ,mType(type)
{
}

std::ostream&
CallEvent::toStream(std::ostream& str) const
{
	str << "CallEvent[";

	switch (mType)
	{
	case outgoing :
		str << "Call outgoing";
		break;
	case cancel :
		str << "Call cancel";
		break;
	case accept :
		str << "Call accept";
		break;
	case peerdown :
		str << "Call peerdown";
		break;
	case reject :
		str << "Call reject";
		break;
	case end :
		str << "Call end";
		break;
	}

	str << "]";

	return str;
}
