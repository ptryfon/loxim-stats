include ../make.defs

OBJECTS	=	QueryParser.o

HEADERS	=	QueryParser.h

MODULE	=	QueryParser.a
PROGRAM	=	

all: $(MODULE) $(PROGRAM)

$(MODULE): $(OBJECTS) $(HEADERS)
	$(AR) r $(MODULE) $(OBJECTS)

#$(PROGRAM): $(MODULE)
#	$(CPP) $(CXXFLAGS) $@.cpp $< -o $@

%.o: %.cpp
	$(CPP) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(MODULE) $(PROGRAM)
