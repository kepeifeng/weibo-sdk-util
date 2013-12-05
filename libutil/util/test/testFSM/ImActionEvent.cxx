#include "stdafx.h"

#include "ImActionEvent.hxx"
#include "ImMachine.hxx"

using namespace Testor;

ImActionEvent::ImActionEvent(Type type,
							 ImMachine& imMachine)
							 :Event(&imMachine)
							 ,mType(type)
{
}

std::ostream&
ImActionEvent::toStream(std::ostream& str) const
{
	str << "IMEvent[";

	switch (mType)
	{
	case im_send :
		str << "IM send";
		break;
	case im_sent :
		str << "IM sent";
		break;
	case im_received :
		str << "IM received";
		break;
	case im_send_failed :
		str << "IM send failed";
		break;
	}

	str << "]";

	return str;
}
