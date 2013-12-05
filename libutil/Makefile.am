# Set the automake options.while execute the automake command, use this option.
AUTOMAKE_OPTIONS=foreign

lib_LTLIBRARIES = libutil.la

SOURCEDIR=$(top_srcdir)/util
INCLUDEDIR=$(top_srcdir)/util

cpp_sources = \
	$(SOURCEDIR)/common/AbstractFifo.cxx\
	$(SOURCEDIR)/common/StlUtil.cxx\
        $(SOURCEDIR)/common/StringUtil.cxx\
        $(SOURCEDIR)/common/TimeUtil.cxx\
        $(SOURCEDIR)/fsm/AddMachineEvent.cxx\
        $(SOURCEDIR)/fsm/Event.cxx\
        $(SOURCEDIR)/fsm/Machine.cxx\
        $(SOURCEDIR)/fsm/MachineSet.cxx\
        $(SOURCEDIR)/fsm/MachineSetHandler.cxx\
        $(SOURCEDIR)/fsm/MachineType.cxx\
        $(SOURCEDIR)/fsm/State.cxx\
        $(SOURCEDIR)/fsm/TimeoutEvent.cxx\
        $(SOURCEDIR)/fsm/TimeoutPredicate.cxx\
        $(SOURCEDIR)/fsm/Transition.cxx\
        $(SOURCEDIR)/fsm/TransitionPredicate.cxx\
        $(SOURCEDIR)/log/Log.cxx\
        $(SOURCEDIR)/log/Logger.cxx\
        $(SOURCEDIR)/log/Subsystem.cxx\
        $(SOURCEDIR)/threading/Condition.cxx\
        $(SOURCEDIR)/threading/Lock.cxx\
        $(SOURCEDIR)/threading/Mutex.cxx\
        $(SOURCEDIR)/threading/RWMutex.cxx\
        $(SOURCEDIR)/threading/ThreadIf.cxx

INCLUDES = -I$(top_srcdir)/../include/libutil -I/opt/local/include

libutil_la_SOURCES = $(cpp_sources)

# This is the destnation include directory of install
libutilincludedir = $(includedir)/libutil/util

# This is the destnation library directory of install
libdir = $(prefix)/lib/$(PLATFORM_NAME)

libutilinclude_HEADERS= \
	$(INCLUDEDIR)/common/AbstractFifo.hxx\
        $(INCLUDEDIR)/common/Fifo.hxx\
        $(INCLUDEDIR)/common/StlUtil.hxx\
        $(INCLUDEDIR)/common/StringUtil.hxx\
        $(INCLUDEDIR)/common/TimeUtil.hxx\
        $(INCLUDEDIR)/compat.hxx\
        $(INCLUDEDIR)/delegate/delegate.hxx\
        $(INCLUDEDIR)/delegate/internal/delegateTemplate.hxx\
        $(INCLUDEDIR)/fsm/AddMachineEvent.hxx\
        $(INCLUDEDIR)/fsm/Event.hxx\
        $(INCLUDEDIR)/fsm/Machine.hxx\
        $(INCLUDEDIR)/fsm/MachineSet.hxx\
        $(INCLUDEDIR)/fsm/MachineSetHandler.hxx\
	$(INCLUDEDIR)/fsm/MachineType.hxx\
	$(INCLUDEDIR)/fsm/State.hxx\
	$(INCLUDEDIR)/fsm/TimeoutEvent.hxx\
        $(INCLUDEDIR)/fsm/TimeoutPredicate.hxx\
        $(INCLUDEDIR)/fsm/Transition.hxx\
        $(INCLUDEDIR)/fsm/TransitionPredicate.hxx\
        $(INCLUDEDIR)/log/ExternalLogger.hxx\
        $(INCLUDEDIR)/log/Log.hxx\
        $(INCLUDEDIR)/log/Logger.hxx\
        $(INCLUDEDIR)/log/Subsystem.hxx\
        $(INCLUDEDIR)/policy/thread_policy.hxx\
        $(INCLUDEDIR)/threading/Condition.hxx\
        $(INCLUDEDIR)/threading/Lock.hxx\
        $(INCLUDEDIR)/threading/Mutex.hxx\
        $(INCLUDEDIR)/threading/RWMutex.hxx\
        $(INCLUDEDIR)/threading/ThreadedData.hxx\
        $(INCLUDEDIR)/threading/ThreadIf.hxx


AM_CXXFLAGS=-D_LINUX -DONLY_EPOLL -D_SERVER -pthread
