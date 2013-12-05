#ifndef UTIL_EVENT_HXX
#define UTIL_EVENT_HXX

#include <util/util.h>

#include <ostream>
#include <vector>
using namespace std;

namespace Util
{
   class UTIL_API MachineBase;
   class UTIL_API MachineSet;
   class UTIL_API Event
   {
      friend class UTIL_API MachineSet;
      friend class UTIL_API StateMachine;
      friend class UTIL_API ActionMachine;
   public:
      Event(const vector<MachineBase*>& targetMachines/* = vector<MachineBase*>()*/) throw();
      Event(MachineBase* targetMachine = NULL) throw();
      Event(MachineSet* machineSet) throw()
         : mMachineSet(machineSet)
      {
      }
      virtual ~Event();
      virtual ostream& toStream(ostream& str) const = 0;

      const MachineSet* getMachineSet() const { return mMachineSet; }
      MachineSet* getMachineSet() { return mMachineSet; }
      //friend ostream& operator<<(ostream&, const Event& event);

   public: // members
      typedef vector<MachineBase*> MachinePtrList;
      MachinePtrList mTargetMachines;

   private:
      MachineSet* mMachineSet;
   };

   UTIL_API ostream& operator<<(ostream& str, const Event& event);

} // namespace Util

#endif // #ifndef UTIL_EVENT_HXX