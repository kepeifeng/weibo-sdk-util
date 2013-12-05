#include "stdafx.h"

#include <util/fsm/State.hxx>
#include <util/fsm/Machine.hxx>
#include <cassert>
using namespace Util;
State::State(StateMachine& owner, const char* name, time_t timeoutMS)
:mName(name)
,mTimeoutMS(timeoutMS)
{
   owner.mStates.push_back(this);
}

State::State(StateMachine& owner, const State& copy)
:mName(copy.mName)
,mTimeoutMS(copy.mTimeoutMS)
{
   owner.mStates.push_back(this);
}

//virtual 
State::~State()
{
}

void
State::clearActions()
{
   OnEnter.Clear();
   OnExit.Clear();
}

time_t
State::getTimeout() const
{
   return mTimeoutMS;
}

void
State::setTimeout(time_t newTimeoutMS)
{
   mTimeoutMS = newTimeoutMS;
}

bool 
State::operator==(const State& rhs) const
{
   return this == &rhs;
}

bool 
State::operator!=(const State& rhs) const
{
   return this != &rhs;
}

// Copyright 2004-2008 Purplecomm, Inc.
