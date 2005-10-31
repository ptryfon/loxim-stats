include ../make.defs

OBJECTS	=	transaction.o

HEADERS	=	transaction.h

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
