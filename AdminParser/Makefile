include ../make.defs


SOURCES	=	admin.tab.cpp \
		lex.Admin.cpp \
		AdminTreeNode.cpp \
		AdminExecutableTreeNode.cpp \
		ShutdownTreeNode.cpp \
		AdminExecutor.cpp



GENERATED_SOURCES =	lex.Admin.cpp \
			admin.tab.cpp \
			admin.tab.hpp \
			location.hh \
			position.hh \
			stack.hh

STATIC_LIB =	AdminParser.a

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB) $(PROGRAM)

#$(PROGRAM): $(STATIC_LIB)
#	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< -o $@

clean:
	rm -f $(OBJECTS) $(DEPFILES) $(STATIC_LIB) $(PROGRAM) $(GENERATED_SOURCES)

$(PROGRAM): $(OBJECTS) $(LIBS) $(STATIC_LIB)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -o $@


$(STATIC_LIB): $(OBJECTS) 
	$(AR) r $(STATIC_LIB) $(OBJECTS)
	$(RANLIB) $(STATIC_LIB)

admin.tab.cpp: admin.yy
	$(YACC) -o admin.tab.cpp admin.yy

lex.Admin.cpp: admin.ll
	$(LEX) admin.ll
	mv lex.Admin.cc lex.Admin.cpp

%.o: %.cpp admin.tab.cpp lex.Admin.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.d: %.cpp admin.tab.cpp lex.Admin.cpp
	@echo "Creating dependencies for $< ..."
	@$(CC) -M -MM -I../ $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(DEPFILES)
