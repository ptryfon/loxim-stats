include ../../make.defs

SOURCES	=	AbstractInputStream.cpp \
		AbstractOutputStream.cpp \
		DescriptorInputStream.cpp \
		DescriptorOutputStream.cpp \
		PriorityOutputStream.cpp

HEADERS	=	AbstractInputStream.h \
		AbstractOutputStream.h \
		DescriptorInputStream.h \
		DescriptorOutputStream.h \
		PriorityOutputStream.h


STATIC_LIB =	streams.a

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB)

#$(PROGRAM): $(STATIC_LIB)
#	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< -o $@

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

