include ../make.defs

OBJECTS	=	QueryExecutor.o

HEADERS	=	QueryResult.h

MODULE	=	QueryExecutor.a
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
