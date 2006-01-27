include ../make.defs

SOURCES	=	Transaction.cpp \
		Semaphore.cpp \
		RWUJSemaphore.cpp \
		Mutex.cpp \
		Main.cpp

HEADERS	=	Transaction.h \
		Semaphore.h \
		RWUJSemaphore.h \
		Mutex.h
		
STATIC_LIB =	TransactionManager.a
PROGRAM	= 	Main	

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB) $(PROGRAM)

$(PROGRAM): $(STATIC_LIB)
	make -C ../Errors
	make -C ../Lock
	make -C ../Store	
	make -C ../Log
	make -C ../Config
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< ../Log/Logs.a ../Lock/Lock.a ../Errors/Errors.a ../Config/Config.a ../Store/Store.a -o $@

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
