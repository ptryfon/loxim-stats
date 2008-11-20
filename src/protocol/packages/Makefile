include ../../make.defs

SOURCES	=	ASCByePackage.cpp \
		ASCErrorPackage.cpp \
		ASCOkPackage.cpp \
		ASCPingPackage.cpp \
		ASCPongPackage.cpp \
		ASCSetOptPackage.cpp \
		PackagesFactory.cpp \
		QCExecutePackage.cpp \
		QCStatementPackage.cpp \
		QSExecutingPackage.cpp \
		QSExecutionFinishedPackage.cpp \
		QSStmtParsedPackage.cpp \
		VSCAbortPackage.cpp \
		VSCFinishedPackage.cpp \
		VSCSendValuePackage.cpp \
		VSCSendValuesPackage.cpp \
		WCHelloPackage.cpp \
		WCLoginPackage.cpp \
		WCModePackage.cpp \
		WCPasswordPackage.cpp \
		WSAuthorizedPackage.cpp \
		WSHelloPackage.cpp

HEADERS	=	ASCByePackage.h \
		ASCErrorPackage.h \
		ASCOkPackage.h \
		ASCPingPackage.h \
		ASCPongPackage.h \
		ASCSetOptPackage.h \
		PackagesFactory.h \
		QCExecutePackage.h \
		QCStatementPackage.h \
		QSExecutingPackage.h \
		QSExecutionFinishedPackage.h \
		QSStmtParsedPackage.h \
		VSCAbortPackage.h \
		VSCFinishedPackage.h \
		VSCSendValuePackage.h \
		VSCSendValuesPackage.h \
		WCHelloPackage.h \
		WCLoginPackage.h \
		WCModePackage.h \
		WCPasswordPackage.h \
		WSAuthorizedPackage.h \
		WSHelloPackage.h 

SUBDIRS = 	data

STATIC_LIB =	packages.a

OBJECTS	=	$(SOURCES:.cpp=.o)
DEPFILES =	$(OBJECTS:.o=.d)

all: subdirs $(STATIC_LIB)

#$(PROGRAM): $(STATIC_LIB)
#	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp $< -o $@

subdirs:
	@for i in $(SUBDIRS); do \
		make -C $$i || exit 1; \
	done


clean:
	rm -f $(OBJECTS) $(DEPFILES) $(STATIC_LIB) $(PROGRAM)
	@for i in $(SUBDIRS); do \
		make -C $$i clean || exit 1; \
	done


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
