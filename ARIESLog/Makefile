include ../make.defs

CXXFLAGS=-I./ -I./../ -Wall -fno-exceptions -g

SOURCES	=	Logs.cpp \
		LogRecord.cpp 

HEADERS	=	Logs.h \
		LogRecord.h 

STATIC_LIB =	Logs.a
PROGRAM =	TestLogs

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB)
test: $(PROGRAM)

$(PROGRAM): $(STATIC_LIB)
	make -C ../SystemStats
	make -C ../TransactionManager
	make -C ../Lock
	make -C ../Errors
	make -C ../Store
	make -C ../Config
	make -C ../QueryExecutor
	make -C ../Indexes
	make -C ../QueryParser
	make -C ../TypeCheck
	make -C ../LoximServer
	make -C ../TCPProto
	make -C ../AdminParser
	make -C ../protocol
	
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp ../Store/Store.a ../TransactionManager/TransactionManager.a ../Lock/Lock.a ../Errors/Errors.a ../Config/Config.a ../SystemStats/SystemStats.a ../Indexes/IndexManager.a ../QueryExecutor/QueryExecutor.a ../TCPProto/TCPProto.a ../LoximServer/LoximSession.o ../LoximServer/LoximServer.o ../LoximServer/KeepAlive.o ../LoximServer/Worker.o ../LoximServer/SignalRouter.o ../LoximServer/Clipboard.o ../protocol/layers/layers.a  ../protocol/pstreams/pstreams.a ../protocol/packages/packages.a ../protocol/ptools/ptools.a ../protocol/packages/data/data.a ../protocol/sockets/sockets.a ../protocol/streams/streams.a ../protocol/ptools/ptools.a ../AdminParser/AdminParser.a ../QueryParser/QueryParser.a ../TypeCheck/TypeChecker.a $< -o $@
	#$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp ../Lock/Lock.a ../Errors/Errors.a ../Config/Config.a ../SystemStats/SystemStats.a $< -o $@
##	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp ../Config/Config.a ../SystemStats/SystemStats.a $< -o $@
	
clean:
	rm -f $(OBJECTS) $(DEPFILES) $(STATIC_LIB) $(PROGRAM)

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
