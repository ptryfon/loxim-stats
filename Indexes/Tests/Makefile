include ../../make.defs

#HEADERS	=		Suits.h
SOURCES	=		NodeBuffer.cpp BTreeTest.cpp Tester.cpp testRun.cpp ComparatorTest.cpp ScopeSelection.cpp \
				IntegrationTest.cpp ConnectionThread.cpp ParentName.cpp ImplicitCalling.cpp Syntax.cpp \
				CleanTest.cpp StateSaving.cpp DropNodes.cpp
PROGRAM	=		testRun
STATIC_LIB=		

# do skompilowania i uruchomienia testow wymagany jest zainstalowany program check
# biblioteka programu check (wersja 0.9.3) poszukiwana bedzie w INCLUDED_LIB
INCLUDED_LIB = 	/usr/lib/libcheck.a
TESTED_LIB=		../IndexManager.a

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)
CXXFLAGS=	-Wall -g -I./../../

SUBDIRS	=	Backup \
		QueryExecutor \
		QueryParser \
		Store \
		TransactionManager \
		Errors \
		Config \
		TCPProto \
		Driver \
		Lock \
		Log \
		Indexes \
		SBQLCli \
		Server 		

NEEDED_LIBS = ../../QueryParser/QueryParser.a \
		../../Config/Config.a \
		../../Errors/Errors.a \
		../../TransactionManager/TransactionManager.a \
		../../Log/Logs.a \
		../../Store/Store.a \
		../../QueryExecutor/QueryExecutor.a \
		../../Lock/Lock.a \
		../../TCPProto/TCPProto.a \
		../../Server/Server.a \
		../../TypeCheck/TypeChecker.a
		
LOGS = sbql_errors.log sbqlerror.log

all:	$(PROGRAM)
		@echo done

run: all
		./${PROGRAM}

needed_libs:
	@for i in $(SUBDIRS); do \
		make -C $$i || exit 1; \
	done

$(PROGRAM): $(OBJECTS) $(TESTED_LIB) $(INCLUDED_LIB) $(NEEDED_LIBS)
		$(CXX) $(CXXFLAGS) -I./../ -I./ $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -o $@ 
#-lefence

$(TESTED_LIB):
		make -C ..

clean:
	rm -f $(OBJECTS) $(DEPFILES) $(PROGRAM) $(LOGS)

$(STATIC_LIB): $(OBJECTS)
	$(AR) r $(STATIC_LIB) $(OBJECTS)
	$(RANLIB) $(STATIC_LIB)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.d: %.cpp
	@echo "Creating dependencies for $< ..."
	@$(CC) -M -MM -I./../../  $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(DEPFILES)
