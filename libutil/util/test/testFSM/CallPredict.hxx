#ifndef TESTOR_CALLPREDICT_HXX
#define TESTOR_CALLPREDICT_HXX


#include <util/fsm/TransitionPredicate.hxx>
#include "CallEvent.hxx"

namespace Testor
{

	class  CallPredicate : public Util::IPredicate
	{
	public:
		enum Target{app};
		CallPredicate(CallEvent::Type type,
			Target match);

		virtual bool operator()(const Util::Event* event,
			const Util::MachineBase& machine);

	private:
		const CallEvent::Type mType;
		const Target mMatch;
	};

}

#endif //#ifndef TESTOR_CALLPREDICT_HXX