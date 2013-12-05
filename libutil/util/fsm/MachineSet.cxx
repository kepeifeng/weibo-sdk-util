#include "stdafx.h"

#include <exception>

#include <cassert>
#include <typeinfo>
#include <sstream>

#include <boost/config.hpp>

#include <util/fsm/MachineSet.hxx>
#include <util/fsm/Machine.hxx>
#include <util/fsm/MachineSetHandler.hxx>

#include <util/fsm/AddMachineEvent.hxx>
#include <util/fsm/TimeoutEvent.hxx>
#include <util/threading/Lock.hxx>
using namespace Util;

MachineSet::MachineSet() throw()
:
#ifdef _WIN32
# if !defined(HAVE_LIBPTHREAD)
mOwnerThreadId(::GetCurrentThreadId())
# else
mOwnerThreadId(::pthread_self().p)
# endif
#else
mOwnerThreadId((void*)::pthread_self())
#endif
{
}

MachineSet::~MachineSet()
{
   mFifo.clear();
}

void MachineSet::registerHandler(boost::shared_ptr<Util::MachineSetHandler> handler)
{
   assert(!(mEventHandler && handler) && "Are you forget reset mEventHandler ?");
   mEventHandler = handler;
}

void
MachineSet::addMachine(MachineBase* machine)
{
   EDebugLog("Adding machine: " << machine->getName() << "(" << machine->getType().getName() << ")");
   assert(!getMachine(machine->getType(), machine->getName()));

#ifdef USING_USTL
   if (mMachinesSet.insert(machine) != mMachinesSet.end())
#else
   if (mMachinesSet.insert(machine).second)
#endif
   {
      mMachines.push_back(machine);
      machine->EnableLogDelegate = EnableLogDelegate;
      machine->CritLogDelegate = CritLogDelegate;
      machine->ErrLogDelegate = ErrLogDelegate;
      machine->WarningLogDelegate = WarningLogDelegate;
      machine->InfoLogDelegate = InfoLogDelegate;
      machine->DebugLogDelegate = DebugLogDelegate;
   }
}

void
MachineSet::removeMachine(MachineBase* machine)
{
   EDebugLog("Removing machine: " << machine->getName() << "(" << machine->getType().getName() << ")");

#ifdef USING_USTL
   if (mMachinesSet.insert(machine) != mMachinesSet.end())
#else
   if (mMachinesSet.erase(machine))
#endif
   {
      for (MachinePtrList::iterator i = mMachines.begin();
         i != mMachines.end(); ++i)
      {
         if ((*i) == machine)
         {
            mMachines.erase(i);
            return;
         }
      }
   }
   //assert(false);
}

MachineBase*
MachineSet::getMachine(const MachineType& type, const string& name)
{
   for (MachinePtrList::const_iterator i = mMachines.begin();
      i != mMachines.end(); ++i)
   {
      if ((*i)->getType() == type &&
         (*i)->getName() == name)
      {
         return *i;
      }
   }
   return 0;
}

//__declspec(deprecated("Deprecated Warning: should use enqueue(boost::shared_ptr<Event>)"))
void
MachineSet::enqueue(Event* event)
{
   enqueue(boost::shared_ptr<Event>(event));
}

void
MachineSet::enqueue(auto_ptr<Event> event)
{
   enqueue(boost::shared_ptr<Event>(event.release()));
}

void
MachineSet::enqueue(boost::shared_ptr<Event> event)
{
   EInfoLog("enqueue " << *event);

   event->mMachineSet = this;
#ifdef ENQUEUE_DIRECT_IF_SAME_THREAD
#ifdef _WIN32
# if !defined(HAVE_LIBPTHREAD)
   DWORD curThreadId = ::GetCurrentThreadId();
# else
   void* curThreadId = ::pthread_self().p;
# endif
#else
   void* curThreadId = ::pthread_self();
#endif

   if (mOwnerThreadId == curThreadId)
   {
      process(event);
   }
   else
#endif
   {
      if (mEventHandler && mEventHandler->onEventEnqueue(event) == MachineSetHandler::MS_Skip)
      {
         // skip
         return ;
      }
      mFifo.add(event);
   }
}

void 
MachineSet::enqueueDirect(Event* event)
{
   enqueueDirect(boost::shared_ptr<Event>(event));
}

void 
MachineSet::enqueueDirect(boost::shared_ptr<Event> event)
{
   EInfoLog("enqueue direct " << *event);

#ifdef _WIN32
# if !defined(HAVE_LIBPTHREAD)
   DWORD curThreadId = ::GetCurrentThreadId();
# else
   void* curThreadId = ::pthread_self().p;
# endif
#else
   void* curThreadId = (void*)::pthread_self();
#endif

   if (mOwnerThreadId == curThreadId)
   {
      event->mMachineSet = this;
      process(event);
   }
   else
   {
      EErrLog("enqueue direct " << "failed: " << "not owner thread");
      enqueue(event);
   }
}

void 
MachineSet::enqueueDirect(auto_ptr<Event> event)
{
   enqueueDirect(boost::shared_ptr<Event>(event.release()));
}

bool
MachineSet::hasEvents() const
{
   if (mEventHandler)
   {
      return mFifo.messageAvailable();
   }
   else
   {
      if (mFifo.messageAvailable())
      {
         return true;
      }

      // check each machine for timeout !dlb! lame scaling!
      for (MachinePtrList::const_iterator i = mMachines.begin();
         i != mMachines.end(); ++i)
      {
         if ((*i)->getTimeout() != 0)
         {
            return true;
         }
      }

      return false;
   }
}

void
MachineSet::process()
{
   while (mFifo.messageAvailable())
   {
      process(mFifo.getNext());
   }

   // check each machine for timeout !dlb! lame scaling!
   if (!mEventHandler)
   {
      for (MachinePtrList::iterator i = mMachines.begin();
         i != mMachines.end(); ++i)
      {
         if ((*i)->isTimeout())
         {
            TimeoutEvent timeout(this, (*i)->getType(), (*i)->getName());
            (*i)->setTimeout((time_t)0);
            (*i)->process(&timeout);
         }
      }
   }
}

bool MachineSet::processTargetMachineEvent(boost::shared_ptr<Event>& delEvent)
{
   bool handled = false;
   for (Event::MachinePtrList::iterator i = delEvent->mTargetMachines.begin();
      i != delEvent->mTargetMachines.end(); ++i)
   {
      MachinePtrSet::iterator found = mMachinesSet.find(*i);
      if (found != mMachinesSet.end())
      {
         assert(*found);
         handled |= (*found)->process(delEvent.get());
      }
   }
   return handled;
}

bool MachineSet::processNoTargetMachineEvent(boost::shared_ptr<Event>& delEvent)
{
   bool handled = false;
   // !NASH! reverse machines traverse
   for (MachinePtrList::reverse_iterator i = mMachines.rbegin();
      i != mMachines.rend(); ++i)
   {
      assert(*i);
      handled |= (*i)->process(delEvent.get());
   }
   return handled;
}

void MachineSet::process(boost::shared_ptr<Event> event)
{
   _TRY_BEGIN
    boost::shared_ptr<AddMachineEvent> addMachineEvent(event, boost::detail::dynamic_cast_tag());

      if (addMachineEvent)
      {
         addMachine(addMachineEvent->getMachine());
         return;
      }
      else
      {
          ;
          
          boost::shared_ptr<RemoveMachineEvent> removeMachineEvent(event, boost::detail::dynamic_cast_tag());

         if (removeMachineEvent)
         {
            removeMachine(removeMachineEvent->getMachine());
            return;
         }
      }
      
      EInfoLog("Handling event: " << *event);
      bool handled = false;
      {
         if (event->mTargetMachines.size())
         {
            handled = processTargetMachineEvent(event);
         }
         else
         {
            handled = processNoTargetMachineEvent(event);
         }
      }

      if (!handled)
      {
         EErrLog("Unhandled event: " << *event);
      }
   _CATCH (exception& e)
#if _HAS_EXCEPTIONS
      EErrLog(__FUNCTION__ << " | caught exception: " << e.what());
      OnProcessError();
#endif
   _CATCH_ALL
#if _HAS_EXCEPTIONS
      EErrLog(__FUNCTION__ << " | caught unknown exception");
      OnProcessError();
#endif
   _CATCH_END
}

void MachineSet::processTimeoutMachine(const MachineBase* machine)
{
   MachinePtrSet::iterator found = mMachinesSet.find(const_cast<MachineBase*>(machine));
   if (found != mMachinesSet.end())
   {
      StateMachine* stateMachine = dynamic_cast<StateMachine*>(*found);
      if (stateMachine)
      {
         TimeoutEvent timeout(this, stateMachine->getType(), stateMachine->getName());
         stateMachine->setTimeout((time_t)0);
         try
         {
            stateMachine->process(&timeout);
         }
#ifndef BOOST_NO_EXCEPTIONS
         catch (exception& e)
         {
            EErrLog(__FUNCTION__ << " | caught exception: " << e.what());
            OnProcessError();
         }
#endif
         catch (...)
         {
            EErrLog(__FUNCTION__ << " | caught unknown exception");
            OnProcessError();
         }
      }
   }
}

void MachineSet::updateTimeoutMahcine(const MachineBase* machine, time_t msSeconds)
{
   if (mEventHandler && msSeconds != 0)
   {
      mEventHandler->onUpdateMachineTimeOut(this, machine, msSeconds);
   }
}

ostream& 
Util::operator<<(ostream& strm, const MachineSet& ms) 
{
   strm << static_cast<const char*>("Machines: ");
   for (MachineSet::MachinePtrList::const_iterator i = ms.mMachines.begin(); i != ms.mMachines.end(); ++i)
   {
      strm << (*i)->getName() << static_cast<const char*>("(") << (*i)->getType().getName() << static_cast<const char*>("),");
   }
   return strm;
}

// Copyright 2005-2007 Purplecomm, Inc.
