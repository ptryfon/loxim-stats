include ../make.defs

PROGRAM =	XMLIO

SOURCES =	XMLimport.cpp XMLexport.cpp XMLIO.cpp StoreLock.cpp

HEADERS	=	XMLimport.h XMLexport.h StoreLock.cpp

OBJECTS = 	$(SOURCES:.cpp=.o)
	
DEPFILES =	$(OBJECTS:.o=.d) $(PROGRAM).d

LIBS =		../Store/Store.a ../Log/Logs.a ../Config/Config.a ../Errors/Errors.a ../TransactionManager/TransactionManager.a ../Lock/Lock.a

CXXFLAGS +=  -fexceptions
LDFLAGS =	-lxerces-c


all: $(PROGRAM) 

test_xerces :
	./test_xerces.sh

$(PROGRAM): $(OBJECTS) $(LIBS) $(STATIC_LIB)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(OBJECTS) $(DEPFILES) $(PROGRAM)

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
