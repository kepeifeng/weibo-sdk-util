#ifndef UTIL_TIMEOUTPREDICATE_HXX
#define UTIL_TIMEOUTPREDICATE_HXX

#include <util/util.h>

#include <boost/scoped_ptr.hpp>

#include <util/fsm/TransitionPredicate.hxx>

namespace Util 
{
class UTIL_API MachineBase;
class UTIL_API MachineType;

class UTIL_API TimeoutPredicate : public IPredicate
{
   public:
      TimeoutPredicate(const MachineType& sourceMachineType);

      virtual bool operator()(const Event* event,
                              const MachineBase& machine) throw();

   private:
      boost::scoped_ptr<MachineType> mSourceMachineType;
};

} // namespace Util

#endif // #ifndef UTIL_TIMEOUTPREDICATE_HXX