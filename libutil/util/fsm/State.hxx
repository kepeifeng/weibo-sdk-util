#ifndef UTIL_STATE_HXX
#define UTIL_STATE_HXX

#include <util/util.h>

#include <boost/shared_ptr.hpp>

# include <vector>
# include <string>
# include <map>

#include <ctime>

#include <util/delegate/delegate.hxx>

namespace Util
{

class UTIL_API Transition;
class UTIL_API MachineSet;
class UTIL_API MachineType;
class UTIL_API MachineBase;
class UTIL_API StateMachine;
class UTIL_API State;
class UTIL_API Event;

typedef Util::Delegate<void (
   MachineBase&,     // machine 
   State&)       // state
> StateEvent;


class UTIL_API State
{
public: // ctor/dtor
   //State(lua_State* L);
   State(StateMachine& owner, const char* name, time_t timerMS = 0);
   State(StateMachine& owner, const State& copy);
   virtual ~State();

public: // methods
   const char* getName() const { return mName; }
   void clearActions();
   time_t getTimeout() const;
   void setTimeout(time_t newTimeoutMS);

   bool operator==(const State& rhs) const;
   bool operator!=(const State& rhs) const;

public: // events
   StateEvent OnEnter;
   StateEvent OnExit;

private: // members
   friend class StateMachine;
   friend class Transition;

   const char* mName;
   time_t mTimeoutMS;

   vector<Transition*> mTransitions;
private:
   State& operator=(const State&);
};

} // namespace Util

#endif // #ifndef UTIL_STATE_HXX