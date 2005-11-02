include ../make.defs

OBJECTS	=	Transaction.o

HEADERS	=	Transaction.h

MODULE	=	TransactionManager.a
PROGRAM	=	

all: $(MODULE)

$(MODULE): $(OBJECTS) 
	$(AR) r $(MODULE) $(OBJECTS)

#$(PROGRAM): $(MODULE)
#	$(CPP) $(CXXFLAGS) $@.cpp $< -o $@

%.o: %.cpp  $(HEADERS)
	$(CPP) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(MODULE) $(PROGRAM)
