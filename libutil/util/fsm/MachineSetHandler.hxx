#ifndef UTIL_MACHINESETHANDLER_HXX
#define UTIL_MACHINESETHANDLER_HXX

#include <util/util.h>

#include <boost/shared_ptr.hpp>

namespace Util
{

   class UTIL_API Event;
   class UTIL_API MachineSet;
   class UTIL_API MachineBase;
   class UTIL_API MachineSetHandler
   {
   public:
      enum
      {
         MS_Skip, // if skip don't call MachineSet's function
         MS_Continue // if continue ,call MachineSet's function
      };

   public:
      MachineSetHandler();
      virtual ~MachineSetHandler();

      virtual int onEventEnqueue(boost::shared_ptr<Event> event) = 0 ;
      void processEvent(boost::shared_ptr<Util::Event>* eventSharedPtrPtr);

      virtual int onUpdateMachineTimeOut(MachineSet* machineSet, const MachineBase* machine, time_t seconds) = 0;
      void processTimeOutMachine(MachineSet* machineSet, const MachineBase* machine);
   };

   class UTIL_API IMachineSetExternalHandler
   {
   public:
      virtual ~IMachineSetExternalHandler() { }

      virtual void registerHandler(boost::shared_ptr<Util::MachineSetHandler> handler) = 0;
   };

} // namespace Util

#endif // #ifndef UTIL_MACHINESETHANDLER_HXX
