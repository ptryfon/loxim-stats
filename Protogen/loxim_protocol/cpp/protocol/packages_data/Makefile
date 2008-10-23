include ./../../make.defs
SOURCES = \
	Uint8Package.cpp \
	Sint8Package.cpp \
	Uint16Package.cpp \
	Sint16Package.cpp \
	Uint32Package.cpp \
	Sint32Package.cpp \
	Uint64Package.cpp \
	Sint64Package.cpp \
	BoolPackage.cpp \
	DatePackage.cpp \
	TimePackage.cpp \
	DatetimePackage.cpp \
	TimetzPackage.cpp \
	DatetimetzPackage.cpp \
	BobPackage.cpp \
	VarcharPackage.cpp \
	DoublePackage.cpp \
	VoidPackage.cpp \
	LinkPackage.cpp \
	BindingPackage.cpp \
	CollectionPackage.cpp \
	StructPackage.cpp \
	BagPackage.cpp \
	SequencePackage.cpp \
	RefPackage.cpp \
	External_refPackage.cpp \
	DataPackagesFactory.cpp

HEADERS = \
	Uint8Package.h \
	Sint8Package.h \
	Uint16Package.h \
	Sint16Package.h \
	Uint32Package.h \
	Sint32Package.h \
	Uint64Package.h \
	Sint64Package.h \
	BoolPackage.h \
	DatePackage.h \
	TimePackage.h \
	DatetimePackage.h \
	TimetzPackage.h \
	DatetimetzPackage.h \
	BobPackage.h \
	VarcharPackage.h \
	DoublePackage.h \
	VoidPackage.h \
	LinkPackage.h \
	BindingPackage.h \
	CollectionPackage.h \
	StructPackage.h \
	BagPackage.h \
	SequencePackage.h \
	RefPackage.h \
	External_refPackage.h \
	DataPackagesFactory.h

SUBDIRS = \

STATIC_LIB = packages_data.a

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