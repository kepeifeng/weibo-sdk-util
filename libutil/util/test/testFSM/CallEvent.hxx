#ifndef TESTOR_CALLVENT_HXX
#define TESTOR_CALLVENT_HXX

#include <util/fsm/Event.hxx>
#include <util/common/StlUtil.hxx>

namespace Testor
{
	class CallMachine;

	class CallEvent : public Util::Event
	{
	public:
		enum Type{outgoing, reject, cancel, accept, peerdown, end} ;

		CallEvent(Type type, CallMachine& imMachine);

		CallEvent::Type getType() const { return mType; }
		virtual std::ostream& toStream(std::ostream& str) const;

	private:
		Type  mType;
	};
} // Testor

#endif //#ifndef TESTOR_CALLVENT_HXX

