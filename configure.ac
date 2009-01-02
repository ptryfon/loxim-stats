AC_INIT([loxim], [0.01], [loxim-devel@lists.sourceforge.net])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_DISABLE_SHARED
AC_PROG_LIBTOOL

#code responsible for setting the ocmpiled-in verbosity level
AC_ARG_ENABLE([cust-verb], [AS_HELP_STRING([--enable-cust-verb=ARG], [Messages
			of which minimum importance will be compiled. Valid
			values are: DEBUG, INFO, WARNING, ERROR,
			SEVERE_ERROR])])
cust_verb_forced=yes
if test "x$enable_cust_verb" = "xno" -o "x$enable_cust_verb" = "xyes" -o \
"x$enable_cust_verb" = "x" ; then

	#these don't make any sense, so we fall back to the default - INFO
	enable_cust_verb=INFO
	cust_verb_forced=no
fi

if test "x$enable_cust_verb" != "xDEBUG" -a "x$enable_cust_verb" != "xINFO" -a \
"x$enable_cust_verb" != "xWARNING" -a "x$enable_cust_verb" != "xERROR" -a \
"x$enable_cust_verb" != "xSEVERE_ERROR" ; then
	AC_MSG_ERROR([cust-verb should be on of DEBUG, INFO, WARNING, ERROR,
			SEVERE_ERROR])
fi

AC_DEFINE_UNQUOTED([HAVE_VERBOSITY_COMPILE], [EC_V_${enable_cust_verb}], [Compiled-in verbosity level])

AC_CHECK_LIB([readline], [readline], , AC_MSG_ERROR([You don't seem to have readline
			library]))
AC_CHECK_LIB([pthread], [pthread_create], , AC_MSG_ERROR([You don't seem to have
			pthread library!]))
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile src/Makefile src/Log/Makefile src/Lock/Makefile
		src/Errors/Makefile src/Store/Makefile src/ARIESLog/Makefile
		src/Indexes/Makefile src/Indexes/Tests/Makefile
		src/QueryParser/Makefile src/TransactionManager/Makefile
		src/SystemStats/Makefile src/QueryExecutor/Makefile
		src/Client/Makefile src/AdminParser/Makefile
		src/protocol/Makefile src/protocol/auth/Makefile
		src/protocol/pstreams/Makefile src/protocol/packages/Makefile
		src/protocol/packages/data/Makefile
		src/protocol/sockets/Makefile src/protocol/streams/Makefile
		src/protocol/layers/Makefile src/protocol/ptools/Makefile
		src/Server/Makefile src/TypeCheck/Makefile
		src/Config/Makefile src/Util/Makefile Documentation/Makefile])
AC_OUTPUT

if test "x$cust_verb_forced" = "xno" ; then
	AC_MSG_WARN([Not all messages will be compiled, which is probably fine
		     for you (by default only debug messages are not compiled),
		     but if you would like to change this behaviour consult the
		     --enable-cust-verb option.])
fi

