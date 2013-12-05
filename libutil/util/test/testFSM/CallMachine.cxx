#include "stdafx.h"

#include "CallMachine.hxx"
#include "CallEvent.hxx"
#include "CallPredict.hxx"

#include <util/fsm/TransitionPredicate.hxx>

using namespace Testor;
using namespace Util;

const MachineType CallMachine::type("CALL");

CallMachine::CallMachine()
:StateMachine(type, getStaticName())
#pragma warning(disable: 4355)
,ready(*this, "ready")
,inCall(*this, "inCall", 250)
,prepare(*this, "prepare", 60*1000)
,endingCall(*this, "endingCall", 10*1000)
,ready_outgoing(ready, "ready_outgoing", prepare, new CallPredicate(CallEvent::outgoing, CallPredicate::app))
,prepare_inCall(prepare, "prepare_inCall", inCall, new CallPredicate(CallEvent::accept, CallPredicate::app))
,prepare_cancel(prepare, "prepare_inCall", ready, new CallPredicate(CallEvent::reject, CallPredicate::app))
,inCall_cancel(inCall, "inCall_end", endingCall, new CallPredicate(CallEvent::cancel, CallPredicate::app))
,inCall_hangUp(inCall, "inCall_end", endingCall, new CallPredicate(CallEvent::peerdown, CallPredicate::app))
,endingCall_ready(endingCall, "endingCall_ready", ready, new CallPredicate(CallEvent::end, CallPredicate::app))
#pragma warning(default: 4355)
{
	setStartState(ready);
}

CallMachine::~CallMachine()
{

}

const std::string&
CallMachine::getStaticName()
{
	static std::string name("CALL");
	return name;
}