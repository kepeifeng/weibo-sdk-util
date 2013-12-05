#include "stdafx.h"

#include <util/fsm/Event.hxx>

using namespace Util;

Event::Event(const vector<MachineBase*>& targetMachines) throw()
:mTargetMachines(targetMachines)
,mMachineSet(0)
{
}

Event::Event(MachineBase* targetMachine) throw()
:mMachineSet(0)
{
   if (targetMachine)
   {
      mTargetMachines.push_back(targetMachine);
   }
}

Event::~Event()
{
}

ostream& 
Util::operator<<(ostream& strm, const Event& event)
{
   return event.toStream(strm);
}

// Copyright 2004-2006 Purplecomm, Inc.

