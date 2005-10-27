include ../make.defs

OBJECTS	=	DBDataValue.o \
		DBLogicalID.o \
		DBObjectPointer.o \
		DBPhysicalID.o \
		DBStoreManager.o

HEADERS	=	DBDataValue.h \
		DBLogicalID.h \
		DBObjectPointer.h \
		DBPhysicalID.h \
		DBStoreManager.h \
		Store.h

MODULE	=	Store.a
PROGRAM	=	Main

all: $(MODULE) $(PROGRAM)

$(MODULE): $(OBJECTS) $(HEADERS)
	$(AR) r $(MODULE) $(OBJECTS)

$(PROGRAM): $(MODULE)
	$(CPP) $(CXXFLAGS) $@.cpp $< -o $@

%.o: %.cpp
	$(CPP) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(MODULE) $(PROGRAM)
