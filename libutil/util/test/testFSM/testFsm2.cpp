// testFsm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <util/fsm/Machine.hxx>
#include <util/fsm/MachineSet.hxx>
#include <util/fsm/Transition.hxx>
#include <boost/shared_ptr.hpp>
#include "CallMachine.hxx"
#include "CallEvent.hxx"

void ready_incall(Util::MachineBase& machine,
				   const Util::ITransition& transition,
				   const Util::Event* event)
{
	std::cout << "ready_incall" << std::endl;
}
void ready_reject(Util::MachineBase& machine,
						  const Util::ITransition& transition,
						  const Util::Event* event)
{
	std::cout << "ready_reject" << std::endl;
}
void incall_end(Util::MachineBase& machine,
				   const Util::ITransition& transition,
				   const Util::Event* event)
{
	std::cout << "incall_end" << std::endl;
}
void ready_cancel(Util::MachineBase& machine,
					   const Util::ITransition& transition,
					   const Util::Event* event)
{
	std::cout << "ready_cancel" << std::endl;
}
void
onEnterReadyState(Util::MachineBase& machine,
								  const  Util::State& state)
{
	std::cout << "onEnterReadyState" << std::endl;
}
void
onExitReadyState(Util::MachineBase& machine,
				  const  Util::State& state)
{
	std::cout << "onExitReadyState" << std::endl;
}
void
onEnteInCallyState(Util::MachineBase& machine,
				  const  Util::State& state)
{
	std::cout << "onEnteInCallyState" << std::endl;
}
void
onExitInCallState(Util::MachineBase& machine,
				  const  Util::State& state)
{
	std::cout << "onExitInCallState" << std::endl;
}
void
onEnterPrepareState(Util::MachineBase& machine,
				  const  Util::State& state)
{
	std::cout << "onEnterPrepareState" << std::endl;
}
void
onExitPrepareState(Util::MachineBase& machine,
				  const  Util::State& state)
{
	std::cout << "onExitPrepareState" << std::endl;
}
void
onEnterEndingCallState(Util::MachineBase& machine,
				  const  Util::State& state)
{
	std::cout << "onEnterEndingCallState" << std::endl;
}
void
onExitEndingCallState(Util::MachineBase& machine,
				  const  Util::State& state)
{
	std::cout << "onExitEndingCallState" << std::endl;
}
void 
ready_outgoing(Util::MachineBase& machine,
											const Util::State& fromState,
											const Util::ITransition& transition,
											const Util::Event* event,
											const Util::State& toState)
{
	std::cout << "ready_outgoing" << std::endl;

}
void 
prepare_inCall(Util::MachineBase& machine,
			   const Util::State& fromState,
			   const Util::ITransition& transition,
			   const Util::Event* event,
			   const Util::State& toState)
{
	std::cout << "prepare_inCall" << std::endl;

}
void 
prepare_cancel(Util::MachineBase& machine,
			   const Util::State& fromState,
			   const Util::ITransition& transition,
			   const Util::Event* event,
			   const Util::State& toState)
{
	std::cout << "prepare_cancel" << std::endl;

}
void 
inCall_hangUp(Util::MachineBase& machine,
			   const Util::State& fromState,
			   const Util::ITransition& transition,
			   const Util::Event* event,
			   const Util::State& toState)
{
	std::cout << "inCall_hangUp" << std::endl;

}
void 
inCall_cancel(Util::MachineBase& machine,
			   const Util::State& fromState,
			   const Util::ITransition& transition,
			   const Util::Event* event,
			   const Util::State& toState)
{
	std::cout << "inCall_cancel" << std::endl;

}
void 
endingCall_ready(Util::MachineBase& machine,
			  const Util::State& fromState,
			  const Util::ITransition& transition,
			  const Util::Event* event,
			  const Util::State& toState)
{
	std::cout << "endingCall_ready" << std::endl;

}
int _tmain(int argc, _TCHAR* argv[])
{
	boost::shared_ptr<Testor::CallMachine> callMachinePtr(new Testor::CallMachine());
	callMachinePtr->ready.OnEnter = onEnterReadyState;
	callMachinePtr->ready.OnExit = onExitReadyState;
	callMachinePtr->inCall.OnEnter = onEnteInCallyState;
	callMachinePtr->inCall.OnExit = onExitInCallState;
	callMachinePtr->prepare.OnEnter = onEnterPrepareState;
	callMachinePtr->prepare.OnExit = onExitPrepareState;
	callMachinePtr->endingCall.OnEnter = onEnterEndingCallState;
	callMachinePtr->endingCall.OnExit = onExitEndingCallState;

	callMachinePtr->ready_outgoing.OnTransition = ready_outgoing;
	callMachinePtr->prepare_inCall.OnTransition = prepare_inCall;
	callMachinePtr->prepare_cancel.OnTransition = prepare_cancel;
	callMachinePtr->inCall_cancel.OnTransition = inCall_cancel;
	callMachinePtr->inCall_hangUp.OnTransition = inCall_hangUp;
	callMachinePtr->endingCall_ready.OnTransition = endingCall_ready;

	boost::shared_ptr<Util::MachineSet> machineSetPtr(new Util::MachineSet());
	machineSetPtr->addMachine(callMachinePtr.get());


	machineSetPtr->enqueue(new Testor::CallEvent(Testor::CallEvent::outgoing, *callMachinePtr));
	_gettchar();
	machineSetPtr->process();

	machineSetPtr->enqueue(new Testor::CallEvent(Testor::CallEvent::accept, *callMachinePtr));
	_gettchar();
	machineSetPtr->process();

	machineSetPtr->enqueue(new Testor::CallEvent(Testor::CallEvent::peerdown, *callMachinePtr));
	_gettchar();
	machineSetPtr->process();

	machineSetPtr->enqueue(new Testor::CallEvent(Testor::CallEvent::end, *callMachinePtr));
	_gettchar();
	machineSetPtr->process();
	_gettchar();

	callMachinePtr->ready_outgoing.clearActions();
	callMachinePtr->prepare_inCall.clearActions();
	callMachinePtr->prepare_cancel.clearActions();
	callMachinePtr->inCall_cancel.clearActions();
	callMachinePtr->inCall_hangUp.clearActions();
	callMachinePtr->endingCall_ready.clearActions();

	callMachinePtr->ready.clearActions();
	callMachinePtr->inCall.clearActions();
	callMachinePtr->prepare.clearActions();
	callMachinePtr->endingCall.clearActions();

	machineSetPtr->removeMachine(callMachinePtr.get());
	_gettchar();
	return 0;
}

