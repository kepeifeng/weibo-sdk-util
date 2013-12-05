#ifndef UTIL_TRANSITIONPREDICATE_HXX
#define UTIL_TRANSITIONPREDICATE_HXX

#include <util/util.h>

namespace Util
{

class UTIL_API Event;
class UTIL_API MachineType;
class UTIL_API MachineBase;

class UTIL_API IPredicate
{
   public:
      virtual ~IPredicate();
      virtual bool operator() (const Event*,
                              const MachineBase& machine) throw() = 0;
};

// 
template <typename EventType>
class SimplePredicate : public IPredicate
{
public:
   typedef typename EventType::Type EventEnum;

   SimplePredicate(EventEnum type) throw()
      : mType(type)
   {}

   virtual bool operator() (const Event* event,
      const MachineBase& machine) throw()
   {
      const EventType* levent = dynamic_cast<const EventType*>(event);
      return (levent && levent->getType() == mType);
   }

private:
   EventEnum mType;
};

} // namespace Util

#endif // #ifndef UTIL_TRANSITIONPREDICATE_HXX
