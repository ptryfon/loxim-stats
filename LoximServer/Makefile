include ../make.defs


PROGRAM	=	Server

SOURCES	=	Server.cpp \
		LoximSession.cpp \
		LoximServer.cpp \
		Clipboard.cpp \
		KeepAlive.cpp \
		SignalRouter.cpp \
		Worker.cpp

HEADERS	= 	../Driver \
		../protocol \
		../SystemStats


OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(SOURCES:.cpp=.d) $(PROGRAM).d

LIBS	=	../Errors/Errors.a \
		../Log/Logs.a \
		../QueryParser/QueryParser.a \
		../Store/Store.a \
		../SystemStats/SystemStats.a \
		../Config/Config.a \
		../TransactionManager/TransactionManager.a \
		../QueryExecutor/QueryExecutor.a \
		../Lock/Lock.a \
		../Indexes/IndexManager.a \
		../TypeCheck/TypeChecker.a \
		../protocol/auth/auth.a \
		../protocol/pstreams/pstreams.a \
		../protocol/packages/data/data.a \
		../protocol/packages/packages.a \
		../protocol/sockets/sockets.a \
		../protocol/streams/streams.a \
		../protocol/layers/layers.a \
		../protocol/ptools/ptools.a \
		../AdminParser/AdminParser.a 



all: $(STATIC_LIB) $(PROGRAM)

#$(PROGRAM): $(OBJECTS) $(LIBS) $(STATIC_LIB)
#	$(CXX) $(LDFLAGS) $^ -o $@ 

$(PROGRAM): $(OBJECTS) $(LIBS) $(STATIC_LIB)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -o $@

clean:
	rm -f $(OBJECTS) $(DEPFILES) $(STATIC_LIB) $(PROGRAM) $(PROGRAM).o 

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

