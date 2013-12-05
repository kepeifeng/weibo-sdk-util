#include "stdafx.h"

#include <algorithm>
#include <cassert>
#include <ctime>
#include <sstream>

#include <boost/config.hpp>

#include <util/fsm/Event.hxx>
#include <util/fsm/Machine.hxx>
#include <util/fsm/MachineSet.hxx>
#include <util/fsm/State.hxx>
#include <util/fsm/Transition.hxx>

using namespace Util;

StateMachine::StateMachine(const MachineType& type,
                           const string& name)
                           : MachineBase(type, name),
                           mTimeoutMS(0),
                           mCurrent(NULL),
                           mPrevious(NULL),
                           mMetaState(NULL)
{
}

StateMachine::StateMachine(const StateMachine& rhs)
:MachineBase(rhs),
mTimeoutMS(rhs.mTimeoutMS),
mCurrent(NULL/*rhs.mCurrent*/),
mPrevious(NULL/*rhs.mPrevious*/),
mMetaState(NULL/*rhs.mMetaState*/)
{
}


StateMachine::~StateMachine()
{
}

bool
StateMachine::forceState(State& state)
{
   if (find(mStates.begin(), mStates.end(), &state) != mStates.end())
   {
      mCurrent = &state;
      return true;
   }
   else
   {
      assert(false);
      return false;
   }
}

bool
StateMachine::setStartState(State& state)
{
   assert(mCurrent == 0);
   if (find(mStates.begin(), mStates.end(), &state) != mStates.end())
   {
      mCurrent = &state;
      return true;
   }
   else
   {
      assert(false);
      return false;
   }
}

bool
StateMachine::setMetaState(State& state)
{
   if (find(mStates.begin(), mStates.end(), &state) != mStates.end())
   {
      mMetaState = &state;
      return true;
   }
   else
   {
      assert(false);
      return false;
   }
}

void
StateMachine::clearMetaState()
{
   mMetaState = 0;
}

const State*
StateMachine::getCurrent() const
{
   return mCurrent;
}

void
StateMachine::setTimeout(unsigned long long timeoutMS)
{
   if (timeoutMS != 0)
   {
      mTimeoutMS = TimeUtil::getTimeMs() + timeoutMS;
   }
   else if (mTimeoutMS)
   {
      mTimeoutMS = 0;
   }
}

unsigned long long
StateMachine::getTimeout() const
{
   return mTimeoutMS;
}

bool
StateMachine::isTimeout() const
{
   if (mTimeoutMS == 0)
   {
      return false;
   }
   else
   {
      return (TimeUtil::getTimeMs() > mTimeoutMS);
   }
}

bool
StateMachine::processNormalStateTransition(Event* event)
{
   if (mCurrent)
   {
      Transition* transition = NULL;
      for (vector<Transition*>::iterator i = mCurrent->mTransitions.begin();
         i != mCurrent->mTransitions.end(); ++i)
      {
         transition = *i;
         if (transition->isMatch(event, *this))
         {
            EInfoLog("Event (match) type: " << event << ", Type: " << mType.getName() << ", Name: " << mName);

            if (transition->getTransitionType() == Transition::tt_NormalTransition)
            {
               EDebugLog("Entering Exit action: (" << mType.getName() << " " << mName << ") " << mCurrent->getName());
               try
               {
                  mCurrent->OnExit(*this, *mCurrent);
                  EDebugLog("Exited Exit action: (" << mType.getName() << " " << mName << ") " << mCurrent->getName());
               }
               catch (...)
               {
                  EErrLog("Caught exception at Exit action: (" << mType.getName() << " " << mName << ") " << mCurrent->getName());
#ifndef BOOST_NO_EXCEPTIONS
                  throw;
#endif
               }
            }

            if (transition->getTransitionType() == Transition::tt_NormalTransition)
            {
               EInfoLog("Normal Transition from: " << mCurrent->getName()
                  << " -> " << transition->mTo->getName()
                  << ", for Machine"
                  << " (" << mType.getName() << " " << mName << ") ");
               EDebugLog("Transition: ["
                  << transition->getName()
                  << "]");
            }
            else
            {
               EInfoLog("Internal Transition for state: " << mCurrent->getName()
                  << ", for Machine"
                  << " (" << mType.getName() << " " << mName << ") ");
               EDebugLog("Transition: ["
                  << transition->getName()
                  << "]");
            }

            try
            {
               transition->OnTransition(
                  *this,
                  *mCurrent,
                  *transition,
                  event,
                  *(transition->mTo));
               if (transition->getTransitionType() == Transition::tt_NormalTransition)
               {
                  EDebugLog("Exited Normal Transition: ["
                     << transition->getName()
                     << "] from: " << mCurrent->getName()
                     << " -> " << transition->mTo->getName()
                     << ", for Machine"
                     << " (" << mType.getName() << " " << mName << ") ");
               }
               else
               {
                  EDebugLog("Exited Internal Transition: ["
                     << transition->getName()
                     << "] for state: " << mCurrent->getName()
                     << ", for Machine"
                     << " (" << mType.getName() << " " << mName << ") ");
               }
            }
            catch (...)
            {
               if (transition->getTransitionType() == Transition::tt_NormalTransition)
               {
                  EErrLog("Caught exception at Normal Transition action: ["
                     << transition->getName()
                     << "] from: " << mCurrent->getName()
                     << " -> " << transition->mTo->getName()
                     << ", for Machine"
                     << " (" << mType.getName() << " " << mName << ") ");
               }
               else
               {
                  EErrLog("Caught exception at Internal Transition action: ["
                     << transition->getName()
                     << "] for state: " << mCurrent->getName()
                     << ", for Machine"
                     << " (" << mType.getName() << " " << mName << ") ");
               }
#ifndef BOOST_NO_EXCEPTIONS
               throw;
#endif
            }
            mPrevious = mCurrent;
            mCurrent = transition->mTo;

            if (transition->getTransitionType() == Transition::tt_NormalTransition)
            {
               // reset timer only on entry
               if (event->mMachineSet)
               {
                  event->mMachineSet->updateTimeoutMahcine(this, mCurrent->getTimeout());
                  setTimeout(mCurrent->getTimeout());
               }
               EDebugLog("Entering Enter action: (" << mType.getName() << " " << mName << ") " << transition->mTo->getName());
               try
               {
                  mCurrent->OnEnter(*this, *mCurrent);
                  EDebugLog("Exited Enter action: (" << mType.getName() << " " << mName << ") " << transition->mTo->getName());
               }
               catch (...)
               {
                  EErrLog("Caught exception at Enter action: (" << mType.getName() << " " << mName << ") " << mCurrent->getName());
#ifndef BOOST_NO_EXCEPTIONS
                  throw;
#endif
               }
            }
            return true;
         }
         //else
         //{
         //   EDebugLog("For Transition: " << transition->getName() << "| Event (nomatch) type: " << *event << " (" << mType.getName() << " " << mName  << ")");
         //}
      }
   }
   return false;
}

bool
StateMachine::processMetaStateTransition(Event* event)
{
   // check meta transitions AFTER specific transitions
   if (mMetaState && mCurrent)
   {
      for (vector<Transition*>::const_iterator i = mMetaState->mTransitions.begin();
         i != mMetaState->mTransitions.end(); ++i)
      {
         Transition* transition = *i;
         if (transition->isMatch(event, *this))
         {
            if (transition->getTransitionType() == Transition::tt_NormalTransition)
            {
               EDebugLog("Entering Meta Exit action: (" << mType.getName() << " " << mName << ") " << mCurrent->getName());
               try
               {
                  mCurrent->OnExit(*this, *mCurrent);
                  EDebugLog("Exited Meta Exit action: (" << mType.getName() << " " << mName << ") " << mCurrent->getName());
               }
               catch (...)
               {
                  EErrLog("Caught exception at Meta Exit action: (" << mType.getName() << " " << mName << ") " << mCurrent->getName());
#ifndef BOOST_NO_EXCEPTIONS
                  throw;
#endif
               }
            }

            EInfoLog( "Meta Transition from: Empty -> " << transition->mTo->getName()
               << ", for Machine"
               << " (" << mType.getName() << " " << mName << ") ");
            EDebugLog( "Meta Transition: [" << transition->getName()
               << "]");
            try
            {
               transition->OnTransition(
                  *this,
                  *mCurrent,
                  *transition,
                  event,
                  *(transition->mTo));
               EDebugLog( "Exited Meta Transition: [" << transition->getName()
                  << "] from: Empty -> " << transition->mTo->getName()
                  << ", for Machine"
                  << " (" << mType.getName() << " " << mName << ") ");
            }
            catch (...)
            {
               EErrLog( "Caught exception at Meta Transition action: [" << transition->getName()
                  << "] from: Empty -> " << transition->mTo->getName()
                  << ", for Machine"
                  << " (" << mType.getName() << " " << mName << ") ");
#ifndef BOOST_NO_EXCEPTIONS
               throw;
#endif
            }

            //State* previous = mCurrent;
            mCurrent = transition->mTo;

            // transitions to meta-state do not change the current state
            if (transition->getTransitionType() == Transition::tt_NormalTransition)
            {
               // reset timer only on entry
               if (event->mMachineSet)
               {
                  event->mMachineSet->updateTimeoutMahcine(this, (*i)->mTo->getTimeout());
                  setTimeout((*i)->mTo->getTimeout());
               }

               EDebugLog("Entering Meta Enter action: (" << mType.getName() << " " << mName << ") " << mCurrent->getName());
               try
               {
                  mCurrent->OnEnter(*this, *(mCurrent));
                  EDebugLog("Exited Meta Enter action: (" << mType.getName() << " " << mName << ") " << mCurrent->getName());
               }
               catch (...)
               {
                  EErrLog("Caught exception at Meta Enter action: (" << mType.getName() << " " << mName << ") " << mCurrent->getName());
#ifndef BOOST_NO_EXCEPTIONS
                  throw;
#endif
               }
            }
            return true;
         }
      }
   }
   return false;
}


bool
StateMachine::process(Event* event)
{
   EDebugLog("Process StateMachine name: " << getName()<< "| Machine type:"
      <<  getType().getName() << "| Current State: " << (mCurrent?mCurrent->getName():"nil"));
   if (processNormalStateTransition(event))
   {
      return true;
   }
   return processMetaStateTransition(event);
}

bool
ActionMachine::process(Event* event)
{
   EDebugLog("Process ActionMachine name: " << getName()<< "| Machine type:"
      <<  getType().getName());
   for(ActionListType::const_iterator i = mNonTransitiveActions.begin();
      i != mNonTransitiveActions.end(); ++i)
   {
      NonTransitiveAction* nonTransitiveTransition = *i;
      if(nonTransitiveTransition->isMatch(event, *this))
      {
         EInfoLog("Non-transitive action: " << nonTransitiveTransition->getName()
            << " for "
            << " (" << mType.getName() << " " << mName << ") ");
         try
         {
            nonTransitiveTransition->OnAction(
               *this,
               *nonTransitiveTransition,
               event);
            EDebugLog("Exited Non-transitive action: " << nonTransitiveTransition->getName()
               << " for "
               << " (" << mType.getName() << " " << mName << ") ");
         }
         catch (...)
         {
            EErrLog("Caught exception at Non-transitive action: " << nonTransitiveTransition->getName()
               << " for (" << mType.getName() << " " << mName << ") ");
#ifndef BOOST_NO_EXCEPTIONS
            throw;
#endif
         }
         return true;
      }
   }
   return false;
}

// Copyright 2004-2007 Purplecomm, Inc.

