noinst_PROGRAMS = thread_test
noinst_LTLIBRARIES = libUtil.la libUtilTest.la
libUtil_la_SOURCES = smartptr.h Backtrace.h Backtrace.cpp InfLoopThread.h InfLoopThread.cpp Concurrency.h Concurrency.cpp SignalRouter.h SignalRouter.cpp
libUtilTest_la_SOURCES =
libUtilTest_la_LIBADD = libUtil.la ../Errors/libErrors.la
thread_test_SOURCES = thread_test.cpp
thread_test_LDADD = libUtilTest.la
INCLUDES=-I$(top_srcdir)/src
