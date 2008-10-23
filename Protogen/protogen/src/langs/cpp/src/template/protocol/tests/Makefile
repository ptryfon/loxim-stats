include ../../make.defs

SOURCES	= TestSuiteFactory.cpp \
 TestPackagesFactory.cpp \
	
HEADERS	= TestSuiteFactory.h \
 TestPackagesFactory.h \

STATIC_LIB = tests.a

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

DEPLIBS = ../auth/auth.a \
 ../base_packages/base_packages.a \
 ../enums/enums.a \
 ../ptools/ptools.a \
 ../sockets/sockets.a \
 ../streams/streams.a \
 ../packages/packages.a \
 ../pstreams/pstreams.a \
 ../layers/layers.a

all: programs

programs: TestRunnerRec TestRunnerSender

TestRunnerRec: $(STATIC_LIB) TestRunnerRec.cpp $(DEPLIBS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< -Wl,--start-group ${DEPLIBS} -Wl,--end-group -o $@
	
TestRunnerSender: $(STATIC_LIB) TestRunnerSender.cpp $(DEPLIBS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< -Wl,--start-group ${DEPLIBS} -Wl,--end-group -o $@

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
