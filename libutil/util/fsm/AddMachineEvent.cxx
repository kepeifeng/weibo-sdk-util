#include "stdafx.h"

#include <util/fsm/AddMachineEvent.hxx>
#include <util/fsm/Machine.hxx>

using namespace Util;

AddMachineEvent::AddMachineEvent(MachineBase* machine)
   : mMachine(machine)
{
}

AddMachineEvent::~AddMachineEvent()
{
}

MachineBase* 
AddMachineEvent::getMachine() const
{
   return mMachine;
}

ostream& 
AddMachineEvent::toStream(ostream& str) const
{
   str << static_cast<const char*>("AddMachineEvent[")
       << static_cast<const char*>(" ") << mMachine->getName()
       << static_cast<const char*>("]");
   return str;
}

RemoveMachineEvent::RemoveMachineEvent(MachineBase* machine)
   : mMachine(machine)
{
}

RemoveMachineEvent::~RemoveMachineEvent()
{
}

MachineBase*
RemoveMachineEvent::getMachine() const
{
   return mMachine;
}


//virtual
ostream& 
RemoveMachineEvent::toStream(ostream& str) const
{
   str << static_cast<const char*>("RemoveMachineEvent[")
       << static_cast<const char*>(" ") << mMachine->getName()
       << static_cast<const char*>("]");
   return str;
}

OwnerRemoveMachineEvent::OwnerRemoveMachineEvent(boost::shared_ptr<MachineBase> machine)
:RemoveMachineEvent(machine.get())
,mOwnedMachine(machine)
{

}

// Copyright 2004-2008 PurpleComm, Inc.
