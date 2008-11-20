include ../make.defs

CXXFLAGS=-I./ -I./../ -Wall -g

PROGRAM	=	Lsbql

SOURCES	=	LoximClient.cpp \
		Lsbql.cpp \
		ReadlineReader.cpp \
		StatementSimpleParser.cpp \
		ClientConsole.cpp \
		StatementProvider.cpp \
		Authenticator.cpp \
		ConsoleAuthenticator.cpp \
		Aborter.cpp

HEADERS	= 	LoximClient.h \
		ReadlineReader.h \
		StatementSimpleParser.h \
		Authenticator.h \
		ClientConsole.h \
		StatementProvider.h \
		ConsoleAuthenticator.h \
		Aborter.h

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(SOURCES:.cpp=.d) $(PROGRAM).d

LIBS	=	../Config/Config.a \
		../Errors/Errors.a \
		../Log/Logs.a \
		../QueryParser/QueryParser.a \
		../Store/Store.a \
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
		../protocol/ptools/ptools.a



all: $(STATIC_LIB) $(PROGRAM)

#$(PROGRAM): $(OBJECTS) $(LIBS) $(STATIC_LIB)
#	$(CXX) $(LDFLAGS) $^ -o $@ 

$(PROGRAM): $(OBJECTS) $(LIBS) $(STATIC_LIB)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -lreadline -Wl,--start-group $^ -Wl,--end-group -o $@

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




