include ../../../make.defs

SOURCES	=	BagData.cpp \
		BindingData.cpp \
		BOBData.cpp \
		BoolData.cpp \
		CollectionData.cpp \
		DataFactory.cpp \
		DateData.cpp \
		DatePart.cpp \
		DateTimeData.cpp \
		DateTimeTZData.cpp \
		DoubleData.cpp \
		ExtReferenceData.cpp \
		LinkData.cpp \
		ReferenceData.cpp \
		SequenceData.cpp \
		Sint16Data.cpp \
		Sint32Data.cpp \
		Sint64Data.cpp \
		Sint8Data.cpp \
		StructData.cpp \
		TimeData.cpp \
		TimeTZData.cpp \
		Uint16Data.cpp \
		Uint32Data.cpp \
		Uint64Data.cpp \
		Uint8Data.cpp \
		VarcharData.cpp \
		VOIDData.cpp

HEADERS	=	BagData.h \
		BindingData.h \
		BOBData.h \
		BoolData.h \
		CollectionData.h \
		DataFactory.h \
		DataPart.h \
		DateData.h \
		DateTimeData.h \
		DateTimeTZData.h \
		DoubleData.h \
		ExtReferenceData.h \
		LinkData.h \
		ReferenceData.h \
		SequenceData.h \
		Sint16Data.h \
		Sint32Data.h \
		Sint64Data.h \
		Sint8Data.h \
		StructData.h \
		TimeData.h \
		TimeTZData.h \
		Uint16Data.h \
		Uint32Data.h \
		Uint64Data.h \
		Uint8Data.h \
		VarcharData.h \
		VOIDData.h \

STATIC_LIB =	data.a

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB)

#$(PROGRAM): $(STATIC_LIB)
#	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< -o $@

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


