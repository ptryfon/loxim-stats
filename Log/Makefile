include ../make.defs

CXXFLAGS=-I./ -I./../ -Wall -fno-exceptions -g

SOURCES	=	Logs.cpp \
		LogRecord.cpp \
		LogIO.cpp \
		LogThread.cpp \
		LogsGlobals.cpp \
		CrashRecovery.cpp \
		Backup.cpp

HEADERS	=	Logs.h \
		LogRecord.h \
		LogIO.h \
		LogThread.h \
		CrashRecovery.h \
		Backup.h

STATIC_LIB =	Logs.a
PROGRAM	=	TestLogs

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB)
program: $(PROGRAM)

$(PROGRAM): $(STATIC_LIB)
	make -C ../TransactionManager
	make -C ../Lock
	make -C ../Errors
	make -C ../Store
	make -C ../Config
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp ../Store/Store.a ../TransactionManager/TransactionManager.a ../Lock/Lock.a ../Errors/Errors.a ../Config/Config.a $< -o $@

clean:
	rm -f $(OBJECTS) $(DEPFILES) $(STATIC_LIB) $(PROGRAM) core*

$(STATIC_LIB): $(OBJECTS)
	$(AR) r $(STATIC_LIB) $(OBJECTS)
	$(RANLIB) $(STATIC_LIB)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.d: %.cpp
	@echo "Creating dependencies for $< ..."
	@$(CC) -M -MM -I../ $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(DEPFILES)
