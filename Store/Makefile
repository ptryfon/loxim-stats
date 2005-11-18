include ../make.defs

SOURCES	=	DBDataValue.cpp \
		DBLogicalID.cpp \
		DBObjectPointer.cpp \
		DBPhysicalID.cpp \
		DBStoreManager.cpp

HEADERS	=	DBDataValue.h \
		DBLogicalID.h \
		DBObjectPointer.h \
		DBPhysicalID.h \
		DBStoreManager.h \
		Store.h

STATIC_LIB =	Store.a
PROGRAM =	TestStore

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB) $(PROGRAM)

$(PROGRAM): $(STATIC_LIB)
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
