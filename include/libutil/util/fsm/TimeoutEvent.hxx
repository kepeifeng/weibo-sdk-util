#ifndef UTIL_TIMEOUTEVENT_HXX
#define UTIL_TIMEOUTEVENT_HXX

#include <util/util.h>
#include <boost/scoped_ptr.hpp>
#include <iostream>
#include <string>
using namespace std;

#include <util/fsm/Event.hxx>
#include <util/fsm/MachineType.hxx>

namespace Util
{
   class UTIL_API MachineSet;
   class UTIL_API MachineType;

   class UTIL_API TimeoutEvent : public Event
   {
   public:
      TimeoutEvent(MachineSet* machineSet,
         const MachineType& machineType,
         const string& machineName);

      const MachineType& getType() const;
      const string& getMachineName() const;

      virtual ostream& toStream(ostream&) const ;

   private:
      boost::scoped_ptr<MachineType> mMachineType;
      string mMachineName ;
   };

} // namespace Util

#endif // #ifndef UTIL_TIMEOUTEVENT_HXX