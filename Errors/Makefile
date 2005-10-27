include ../make.defs

OBJECTS	=	ErrorConsole.o

HEADERS	=	ErrorConsole.h \
		Errors.h

MODULE	=	Errors.a
PROGRAM	=	

all: $(MODULE)

$(MODULE): $(OBJECTS) $(HEADERS)
	$(AR) r $(MODULE) $(OBJECTS)

#$(PROGRAM): $(MODULE)
#	$(CPP) $(CXXFLAGS) $@.cpp $< -o $@

%.o: %.cpp
	$(CPP) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(MODULE) $(PROGRAM)
