include ../make.defs

CXXFLAGS=-I./ -I./../ -Wall -g

SOURCES	=	SBQLConfig.cpp
			
HEADERS	=	SBQLConfig.h \
			../SystemStats/AllStats.h \
			../SystemStats/ConfigStats.h \


LIBS	=	../SystemStats/SystemStats.a

STATIC_LIB =	Config.a

PROGRAM	=	conftest

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB) $(PROGRAM) $(HEADERS)

#$(PROGRAM): $(STATIC_LIB)
#	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< -o $@

$(PROGRAM): $(STATIC_LIB) $(PROGRAM).cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< $(LIBS) -o $@


clean:
	rm -f $(OBJECTS) $(DEPFILES) $(STATIC_LIB) $(PROGRAM)

$(STATIC_LIB): $(OBJECTS)
	$(AR) r $(STATIC_LIB) $(OBJECTS) $(LIBS)
	$(RANLIB) $(STATIC_LIB)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.d: %.cpp
	@echo "Creating dependencies for $< ..."
	@$(CC) -M -MM -I../ $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(DEPFILES)
