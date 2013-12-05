#include "stdafx.h"

#include <util/fsm/TimeoutPredicate.hxx>

#include <util/fsm/TimeoutEvent.hxx>
#include <util/fsm/Machine.hxx>

using namespace Util;

TimeoutPredicate::TimeoutPredicate(const MachineType& sourceMachineType)
   : mSourceMachineType(new (nothrow) MachineType(sourceMachineType))
{
}

bool
TimeoutPredicate::operator()(const Event* event,
                             const MachineBase& machine) throw()
{
   const TimeoutEvent* tevent = dynamic_cast<const TimeoutEvent*>(event);
   if (!tevent)
   {
      return false;
   }

   if (*mSourceMachineType != tevent->getType())
   {
      return false;
   }

   return machine.getName() == tevent->getMachineName();
}

// Copyright 2004-2006 Purplecomm, Inc.
