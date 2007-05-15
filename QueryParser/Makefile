include ../make.defs

SOURCES	=	QueryParser.cpp \
		TreeNode.cpp \
		DataRead.cpp \
		Stack.cpp \
		Optymalizator.cpp \
		DeathRmver.cpp \
		AuxRmver.cpp \
		Deb.cpp \
		Privilige.cpp \
		parser.tab.cpp \
		lex.yy.cpp

HEADERS	=	QueryParser.h \
		TreeNode.h \
		IndexNode.h \
		Stack.h \
		ClassNames.h \
		DataRead.h \
		Optymalizator.h \
		DeathRmver.h \
		AuxRmver.h \
		Deb.h \
		Privilige.h \
		parser.tab.h
		
GENERATED_SOURCES =	lex.yy.cpp \
					parser.tab.cpp \
					parser.tab.h

STATIC_LIB =	QueryParser.a
PROGRAM	=	

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB) $(PROGRAM)

#$(PROGRAM): $(STATIC_LIB)
#	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< -o $@

clean:
	rm -f $(OBJECTS) $(DEPFILES) $(STATIC_LIB) $(PROGRAM) $(GENERATED_SOURCES)

$(STATIC_LIB): $(OBJECTS)
	$(AR) r $(STATIC_LIB) $(OBJECTS)
	$(RANLIB) $(STATIC_LIB)

parser.tab.cpp: parser.y
	$(YACC) -d -v parser.y
	mv parser.tab.c parser.tab.cpp

lex.yy.cpp: scanner.l parser.tab.cpp
	$(LEX) scanner.l
	mv lex.yy.cc lex.yy.cpp

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.d: %.cpp
	@echo "Creating dependencies for $< ..."
	@$(CC) -M -MM -I../ $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(DEPFILES)
