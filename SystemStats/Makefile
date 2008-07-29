include ../make.defs


PROGRAM	=	

SOURCES	= SessionStats.cpp \
	 StoreStats.cpp \
	 ConfigStats.cpp \
	 TransactionsStats.cpp \
	 SystemStats.cpp \
	 AllStats.cpp

HEADERS	= SessionStats.h \
	StoreStats.h \
	TransactionsStats.h \
	ConfigStats.h \
    SystemStats.h \
    AllStats.h


OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(SOURCES:.cpp=.d) $(PROGRAM).d

LIBS	=	

STATIC_LIB = SystemStats.a

all: $(STATIC_LIB) $(PROGRAM)

#$(PROGRAM): $(OBJECTS) $(LIBS) $(STATIC_LIB)
#	$(CXX) $(LDFLAGS) $^ -o $@ 

$(PROGRAM): $(OBJECTS) $(LIBS) $(STATIC_LIB)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -o $@

clean:
	rm -f $(OBJECTS) $(DEPFILES) $(STATIC_LIB) $(PROGRAM) $(PROGRAM).o 

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

