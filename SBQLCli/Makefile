include ../make.defs

CXXFLAGS=-I./ -I./../ -Wall

PROGRAM	=	SBQLCli

SOURCES	=	SBQLCli.cpp 

HEADERS	= ../Driver/Result.h

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(SOURCES:.cpp=.d) $(PROGRAM).d

LIBS	=	../Driver/Driver.a \
		../Config/Config.a \
		../Errors/Errors.a \
		../Log/Logs.a \
		../QueryParser/QueryParser.a \
		../Store/Store.a \
		../TransactionManager/TransactionManager.a \
		../QueryExecutor/QueryExecutor.a \
		../Lock/Lock.a \
		../TCPProto/TCPProto.a



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

