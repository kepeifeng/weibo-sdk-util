#ifndef TUTIL_MACHINESET_HXX
#define TUTIL_MACHINESET_HXX

#include <vector>
#include <set>
#include <memory>

#include <util/threading/Mutex.hxx>
#include <util/common/Fifo.hxx>

#include <util/delegate/delegate.hxx>

#include <util/log/ExternalLogger.hxx>

namespace Util
{
class UTIL_API MachineSetHandler;
class UTIL_API MachineBase;
class UTIL_API MachineType;
class UTIL_API Event;

class UTIL_API MachineSet : public ExternalLogger
{
   public:
      MachineSet() throw();
      ~MachineSet();

      void registerHandler(boost::shared_ptr<Util::MachineSetHandler> handler);
      // thread safe
#if defined(_WIN32)
      __declspec(deprecated("Deprecated Warning: should use enqueue(boost::shared_ptr<Event>) instead")) 
#endif
      void enqueue(Event* event);
      void enqueue(boost::shared_ptr<Event> event);
      void enqueue(auto_ptr<Event> event);
#if defined(_WIN32)
      __declspec(deprecated("Deprecated Warning: should use enqueueDirect(boost::shared_ptr<Event>) instead")) 
#endif
      void enqueueDirect(Event* event);
      void enqueueDirect(boost::shared_ptr<Event> event);
      void enqueueDirect(auto_ptr<Event> event);
      bool hasEvents() const;
      void process();      
      void process(boost::shared_ptr<Event> event);      
      void processTimeoutMachine(const MachineBase* machine);
      
      // thread safe now!
      void addMachine(MachineBase* machine);
      void removeMachine(MachineBase* machine);
      MachineBase* getMachine(const MachineType& type, const string& name);
      void updateTimeoutMahcine(const MachineBase* machine, time_t timeout) ;
      bool hasHandler() const { return mEventHandler != NULL; }

   public: // events
      NotifyEvent OnProcessError;

private: // methods
      bool processTargetMachineEvent(boost::shared_ptr<Event>& delEvent);
      bool processNoTargetMachineEvent(boost::shared_ptr<Event>& delEvent);

   private:
      typedef vector<MachineBase*> MachinePtrList;
      vector<MachineBase*> mMachines;
      typedef set<MachineBase*> MachinePtrSet;
      set<MachineBase*> mMachinesSet;
      Fifo<Event> mFifo;
      boost::shared_ptr<Util::MachineSetHandler> mEventHandler;

#if defined(_WIN32) && !defined(HAVE_LIBPTHREAD)
      DWORD mOwnerThreadId;
#else
      void* mOwnerThreadId;
#endif

      friend ostream& operator<<(ostream& strm, const MachineSet& ms);
};

ostream& operator<<(ostream& strm, const MachineSet& ms);

}  // namespace Util

#endif // #ifndef UTIL_MACHINESET_HXX
