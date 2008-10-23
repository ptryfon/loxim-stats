include ./../../make.defs
SOURCES = \
	W_c_helloPackage.cpp \
	W_s_helloPackage.cpp \
	W_c_modePackage.cpp \
	W_c_loginPackage.cpp \
	W_c_authorizedPackage.cpp \
	W_c_passwordPackage.cpp \
	Q_c_statementPackage.cpp \
	Q_s_stmtparsedPackage.cpp \
	Q_c_executePackage.cpp \
	Q_s_executingPackage.cpp \
	Q_s_execution_finishedPackage.cpp \
	A_sc_okPackage.cpp \
	A_sc_errorPackage.cpp \
	A_sc_byePackage.cpp \
	A_sc_setoptPackage.cpp \
	V_sc_sendvaluesPackage.cpp \
	V_sc_sendvaluePackage.cpp \
	V_sc_finishedPackage.cpp \
	V_sc_abortPackage.cpp \
	PackagesFactory.cpp

HEADERS = \
	W_c_helloPackage.h \
	W_s_helloPackage.h \
	W_c_modePackage.h \
	W_c_loginPackage.h \
	W_c_authorizedPackage.h \
	W_c_passwordPackage.h \
	Q_c_statementPackage.h \
	Q_s_stmtparsedPackage.h \
	Q_c_executePackage.h \
	Q_s_executingPackage.h \
	Q_s_execution_finishedPackage.h \
	A_sc_okPackage.h \
	A_sc_errorPackage.h \
	A_sc_byePackage.h \
	A_sc_setoptPackage.h \
	V_sc_sendvaluesPackage.h \
	V_sc_sendvaluePackage.h \
	V_sc_finishedPackage.h \
	V_sc_abortPackage.h \
	PackagesFactory.h

SUBDIRS = \

STATIC_LIB = packages.a

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