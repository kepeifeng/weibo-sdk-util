#ifndef TESTOR_CALLMACHINE_HXX
#define TESTOR_CALLMACHINE_HXX

#include <util/fsm/Machine.hxx>
#include <util/fsm/MachineType.hxx>
#include <util/fsm/Transition.hxx>
#include <util/fsm/State.hxx>

namespace Testor
{
	class CallMachine : public Util::StateMachine
	{
	public:
		CallMachine();
		virtual ~CallMachine();

		const std::string& getStaticName();
		static const Util::MachineType type;

		Util::State ready;
		Util::State inCall;
		Util::State prepare;
		Util::State endingCall;

		Util::Transition ready_outgoing;
		Util::Transition prepare_inCall;
		Util::Transition prepare_cancel;
		Util::Transition inCall_cancel;
		Util::Transition inCall_hangUp;
		Util::Transition endingCall_ready;
	};
}//Testor
#endif //#ifndef TESTOR_CALLMACHINE_HXX