include ../make.defs

SOURCES	=	Node.cpp IndexManager.cpp Metadata.cpp IndexHandler.cpp BTree.cpp RootEntry.cpp \
			CachedNode.cpp Monitor.cpp EntrySieve.cpp Constraints.cpp LeafTraveller.cpp Comparator.cpp \
			ToStringBuilder.cpp QueryOptimizer.cpp OptimizedField.cpp VisibilityResolver.cpp \
			TransactionImpact.cpp Cleaner.cpp NonloggedDataSerializer.cpp const.cpp

HEADERS	=	IndexManager.h IndexHandler.h BTree.h RootEntry.h CachedNode.h Monitor.h Node.h LeafTraveller.h \
			Constraints.h Comparator.h QueryOptimizer.h OptimizedField.h

STATIC_LIB =	IndexManager.a
PROGRAM	=	

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB) $(PROGRAM)

#$(PROGRAM): $(STATIC_LIB)
#	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< -o $@

clean:
	rm -f $(OBJECTS) $(DEPFILES) $(STATIC_LIB) $(PROGRAM)
	make -C Tests clean

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
