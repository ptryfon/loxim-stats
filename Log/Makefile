include ../make.defs

SOURCES	=	Logs.cpp \
		LogRecord.cpp \
		LogIO.cpp \
		LogThread.cpp

HEADERS	=	Logs.h \
		LogRecord.h \
		LogIO.h \
		LogThread.h

STATIC_LIB =	Logs.a
PROGRAM	=	TestLogs

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB) $(PROGRAM)

$(PROGRAM): $(STATIC_LIB)
	$(CXX) $(CXXFLAGS) $@.cpp $< ../TransactionManager/TransactionManager.a ../Lock/Lock.a -o $@

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
