#include "stdafx.h"

#include <util/fsm/MachineSetHandler.hxx>

#include <util/fsm/AddMachineEvent.hxx>
#include <util/fsm/Machine.hxx>
#include <util/fsm/MachineSet.hxx>
#include <util/fsm/TimeoutEvent.hxx>

using namespace Util;

MachineSetHandler::MachineSetHandler()
//:mMachineSet(0)
{}

MachineSetHandler::~MachineSetHandler()
{

}

void MachineSetHandler::processEvent(boost::shared_ptr<Util::Event>* eventSharedPtrPtr)
{
   assert(/*mMachineSet && */eventSharedPtrPtr);
   boost::shared_ptr<Util::Event> eventPtr(*eventSharedPtrPtr);
   delete eventSharedPtrPtr;
   assert(eventPtr->getMachineSet());
   eventPtr->getMachineSet()->process(eventPtr);
}

void MachineSetHandler::processTimeOutMachine(MachineSet* machineSet, const MachineBase* machine)
{
   assert(machineSet);
   machineSet->processTimeoutMachine(machine);
}
