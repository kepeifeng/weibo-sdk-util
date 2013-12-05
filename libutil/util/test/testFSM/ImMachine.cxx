#include "stdafx.h"

#include "ImMachine.hxx"
#include "ImActionEvent.hxx"

#include <util/fsm/TransitionPredicate.hxx>

using namespace Testor;
using namespace Util;

const MachineType ImMachine::type("IM");

ImMachine::ImMachine()
:ActionMachine(type, getStaticName())
#pragma warning(disable: 4355)
,imSendAction(*this, "imSendAction", boost::shared_ptr<Util::IPredicate>(new Util::SimplePredicate<ImActionEvent>(ImActionEvent::im_send)))
,imSentAction(*this, "imSentAction", boost::shared_ptr<Util::IPredicate>(new Util::SimplePredicate<ImActionEvent>(ImActionEvent::im_sent)))
,imReceivedAction(*this, "imReceivedAction", boost::shared_ptr<Util::IPredicate>(new Util::SimplePredicate<ImActionEvent>(ImActionEvent::im_received)))
,imSendFailedAction(*this, "imSendFailedAction", boost::shared_ptr<Util::IPredicate>(new Util::SimplePredicate<ImActionEvent>(ImActionEvent::im_send_failed)))
#pragma warning(default: 4355)
{
}

ImMachine::~ImMachine()
{

}

const std::string&
ImMachine::getStaticName()
{
	static std::string name("IM");
	return name;
}