#include "stdafx.h"

#include <cassert>

#include <util/fsm/TimeoutEvent.hxx>

using namespace Util;

TimeoutEvent::TimeoutEvent(MachineSet* machineSet,
                           const MachineType& machineType,
                           const string& machineName)
   :Event(machineSet)
   ,mMachineType(new (nothrow) MachineType(machineType))
   ,mMachineName(machineName)
{
   assert(machineSet);
}

const MachineType&
TimeoutEvent::getType() const
{
   return *mMachineType;
}

const string&
TimeoutEvent::getMachineName() const
{
   return mMachineName;
}

ostream& 
TimeoutEvent::toStream(ostream& str) const
{
   str << static_cast<const char*>("TimeoutEvent[") << mMachineType->getName()
       << static_cast<const char*>(", ") << mMachineName
       << static_cast<const char*>("]");

   return str;
}

// Copyright 2004-2006 Purplecomm, Inc.
