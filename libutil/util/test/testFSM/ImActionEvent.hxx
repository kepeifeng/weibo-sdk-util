#ifndef TESTOR_IMACTIONEVENT_HXX
#define TESTOR_IMACTIONEVENT_HXX

#include <util/fsm/Event.hxx>
#include <util/common/StlUtil.hxx>

namespace Testor
{
	class ImMachine;

	class ImActionEvent : public Util::Event
	{
	public:
		enum Type{im_send, im_sent, im_received, im_send_failed} ;

		ImActionEvent(Type type, ImMachine& imMachine);
			
		ImActionEvent::Type getType() const { return mType; }
		virtual std::ostream& toStream(std::ostream& str) const;

	private:
		Type  mType;
	};
} // Testor

#endif //#ifndef TESTOR_IMACTIONEVENT_HXX

