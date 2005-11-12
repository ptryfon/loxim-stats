include ../make.defs

SOURCES	=	Transaction.cpp \
		Semlib.cpp

HEADERS	=	Transaction.h \
		Semlib.h
		
STATIC_LIB =	TransactionManager.a
PROGRAM	=	

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB)

#$(PROGRAM): $(STATIC_LIB)
#	$(CXX) $(CXXFLAGS) $@.cpp $< -o $@

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
