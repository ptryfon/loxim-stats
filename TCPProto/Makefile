include ../make.defs

CXXFLAGS=-I./ -I./../ -Wall

SOURCES	=	SimpleResultPackage.cpp \
			SimpleQueryPackage.cpp \
			ParamQueryPackage.cpp \
			StatementPackage.cpp \
			ParamStatementPackage.cpp \
			ErrorPackage.cpp \
			RemoteQueryPackage.cpp \
			RemoteResultPackage.cpp \
			Tcp.cpp \
			../Driver/Result.cpp

HEADERS	=	Tcp.h \
			Package.h \
			../Driver/Result.h

STATIC_LIB =	TCPProto.a
PROGRAM =	

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: $(STATIC_LIB) $(PROGRAM) $(HEADERS)

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
