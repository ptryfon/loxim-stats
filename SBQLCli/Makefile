include ../make.defs

SOURCES	=	SBQLCli.cpp
HEADERS	=	

#STATIC_LIB =	SBQLCli.a
PROGRAM	=	client

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(SOURCES:.cpp=.d) $(PROGRAM).d

LIBS	=	../Config/Config.a \
		../Errors/Errors.a \
		../Log/Logs.a \
		../QueryParser/QueryParser.a \
		../Store/Store.a \
		../TransactionManager/TransactionManager.a \
		../QueryExecutor/QueryExecutor.a

all: $(STATIC_LIB) $(PROGRAM)

$(PROGRAM): $(PROGRAM).o $(LIBS) $(STATIC_LIB)
	$(CXX) $^ -o $@

clean:
	rm -f $(OBJECTS) $(DEPFILES) $(STATIC_LIB) $(PROGRAM) $(PROGRAM).o

$(STATIC_LIB): $(OBJECTS)
	$(AR) r $(STATIC_LIB) $(OBJECTS)
	$(RANLIB) $(STATIC_LIB)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.d: %.cpp
	@echo "Creating dependencies..."
	@$(CC) -M -MM -I../ $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(DEPFILES)
