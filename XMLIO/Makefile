include ../make.defs

PROGRAM =	XMLexport

SOURCES	=	XMLexport.cpp 

HEADERS	=	XMLexport.h

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d) $(PROGRAM).d

LIBS =		../Driver/Driver.a

#STATIC_LIB =	XMLexport.a 
LDFLAGS =	-lxerces-c


all: $(STATIC_LIB) $(PROGRAM)

$(PROGRAM): $(OBJECTS) $(LIBS) $(STATIC_LIB)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

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
