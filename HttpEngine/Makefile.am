# Set the automake options.while execute the automake command, use this option.
AUTOMAKE_OPTIONS=foreign

lib_LTLIBRARIES = libHttpEngine.la

SOURCEDIR=$(top_srcdir)/src
INCLUDEDIR=$(top_srcdir)/include

cpp_sources = \
        $(SOURCEDIR)/HashHelper.cxx\
        $(SOURCEDIR)/HEDriver.cxx\
        $(SOURCEDIR)/HEDriverCommand.cxx\
        $(SOURCEDIR)/HEEngineImpl.cxx\
        $(SOURCEDIR)/HESessionInfo.cxx\
        $(SOURCEDIR)/HttpRequestInfo.cxx\
	$(top_srcdir)/../Common/bbiconv.cc\
	$(top_srcdir)/../Common/strconv.cpp\
	$(top_srcdir)/../Common/Urlcode.cpp

INCLUDES = \
	-I$(top_srcdir)/../Common\
	-I$(top_srcdir)/include\
	-I$(top_srcdir)/../include/libutil\
	-I$(top_srcdir)/../include

#-I/opt/local/include

libHttpEngine_la_SOURCES = $(cpp_sources)

# This is the destnation include directory of install
libHttpEngineincludedir = $(includedir)/HttpEngine

# This is the destnation library  directory of install
libdir = $(prefix)/lib/$(PLATFORM_NAME)

libHttpEngineinclude_HEADERS= \
        $(INCLUDEDIR)/HttpCompat.hxx\
        $(INCLUDEDIR)/IHttpEngine.hxx\
        $(INCLUDEDIR)/IHttpEngineCommon.hxx

AM_CXXFLAGS=-D_LINUX -DONLY_EPOLL -D_SERVER -pthread
