#ifndef UTIL_ADDMACHINEEVENT_HXX
#define UTIL_ADDMACHINEEVENT_HXX

#include <util/util.h>

#include <boost/shared_ptr.hpp>

#include <iostream>
using namespace std;

#include <util/fsm/Event.hxx>

namespace Util
{
   class UTIL_API MachineBase;
   class UTIL_API AddMachineEvent : public Event
   {
   public:
      AddMachineEvent(MachineBase* machine);
      ~AddMachineEvent();

      MachineBase* getMachine() const;

      virtual ostream& toStream(ostream& str) const;

   private:
      MachineBase* mMachine;
   };

   class UTIL_API RemoveMachineEvent : public Event
   {
   public:
      RemoveMachineEvent(MachineBase* machine);
      ~RemoveMachineEvent();

      MachineBase* getMachine() const;

      virtual ostream& toStream(ostream& str) const;

   private:
      MachineBase* mMachine;
   };

   class UTIL_API OwnerRemoveMachineEvent : public RemoveMachineEvent
   {
   public:
      OwnerRemoveMachineEvent(boost::shared_ptr<MachineBase> machine);

   private:
      boost::shared_ptr<MachineBase> mOwnedMachine;
   };

} // namespace Util

#endif // #ifndef UTIL_ADDMACHINEEVENT_HXX
