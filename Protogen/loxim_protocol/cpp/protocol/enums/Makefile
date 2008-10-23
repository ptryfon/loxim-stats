include ./../../make.defs
SOURCES = \
	FeaturesFactory.cpp \
	Auth_methodsFactory.cpp \
	CollationsFactory.cpp \
	ModesFactory.cpp \
	Statement_flagsFactory.cpp \
	ErrorsFactory.cpp \
	Bye_reasonsFactory.cpp \
	Send_value_flagsFactory.cpp \
	Abort_reason_codesFactory.cpp

HEADERS = \
	FeaturesFactory.h \
	Auth_methodsFactory.h \
	CollationsFactory.h \
	ModesFactory.h \
	Statement_flagsFactory.h \
	ErrorsFactory.h \
	Bye_reasonsFactory.h \
	Send_value_flagsFactory.h \
	Abort_reason_codesFactory.h

SUBDIRS = \

STATIC_LIB = enums.a

OBJECTS =       $(SOURCES:.cpp=.o)
DEPFILES =      $(OBJECTS:.o=.d)

all: subdirs $(STATIC_LIB)

#$(PROGRAM): $(STATIC_LIB)
#       $(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< -o $@

clean: clean-subdirs
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

subdirs:
	@for i in $(SUBDIRS); do \
		make -C $$i || exit 1; \
	done

clean-subdirs:
	@find -name "*.d" | xargs rm -f
	@for i in $(SUBDIRS); do \
		make -C $$i clean || exit 1; \
	done