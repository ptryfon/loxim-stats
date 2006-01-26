include ../make.defs

SOURCES	=	ErrorConsole.cpp \
		ErrorMsg.cpp

HEADERS	=	ErrorConsole.h \
		Errors.h

STATIC_LIB =	Errors.a
PROGRAM	=	consoletest

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d) $(PROGRAM).d

all: $(STATIC_LIB) $(PROGRAM)

$(PROGRAM): $(STATIC_LIB) $(PROGRAM).cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< -o $@

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
