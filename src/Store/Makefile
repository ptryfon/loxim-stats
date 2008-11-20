include ../make.defs

CXXFLAGS=-I./ -I./../ -Wall -fno-exceptions -g

SOURCES	=	Buffer.cpp \
		DBDataValue.cpp \
		DBLogicalID.cpp \
		DBObjectPointer.cpp \
		DBPhysicalID.cpp \
		DBStoreManager.cpp \
		File.cpp \
		Map.cpp \
		NamedRoots_new.cpp \
		Views.cpp \
		NamedItems.cpp \
		Classes.cpp \
		Interfaces.cpp \
		PagePointer.cpp \
		PageManager.cpp \
		Misc.cpp \
		SystemViews.cpp

HEADERS	=	../QueryExecutor/HashMacro.h \
			../SystemStats/AllStats.h \
			../SystemStats/SystemStats.h \
		Buffer.h \
		DBDataValue.h \
		DBLogicalID.h \
		DBObjectPointer.h \
		DBPhysicalID.h \
		DBStoreManager.h \
		File.h \
		Map.h \
		NamedItems.h \
		Views.h \
		PagePointer.h \
		PageManager.h \
		Store.h \
		Misc.h \
		Struct.h \
		SystemViews.h


STATIC_LIB =	Store.a
PROGRAM =	TestStore

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB)
test: $(PROGRAM)

$(PROGRAM): $(STATIC_LIB) $(PROGRAM).cpp
	make -C ../TransactionManager
	make -C ../SystemStats
	make -C ../Lock
	make -C ../Errors
	make -C ../Log
	make -C ../Config
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< ../TransactionManager/TransactionManager.a ../Lock/Lock.a ../Errors/Errors.a ../Log/Logs.a ../Config/Config.a -o $@

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
