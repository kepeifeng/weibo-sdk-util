#ifndef TESTOR_IMMACHINE_HXX
#define TESTOR_IMMACHINE_HXX

#include <util/fsm/Machine.hxx>
#include <util/fsm/MachineType.hxx>
#include <util/fsm/Transition.hxx>

namespace Testor
{
	class ImMachine : public Util::ActionMachine
	{
	public:
		ImMachine();
		virtual ~ImMachine();

		const std::string& getStaticName();
		static const Util::MachineType type;

		Util::NonTransitiveAction imSendAction;
		Util::NonTransitiveAction imSentAction;
		Util::NonTransitiveAction imReceivedAction;
		Util::NonTransitiveAction imSendFailedAction;
	};

} // Testor
#endif //#ifndef TESTOR_IMMACHINE_HXX