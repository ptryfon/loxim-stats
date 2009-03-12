AC_INIT([loxim], [0.01], [loxim-devel@lists.sourceforge.net])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LEX
AC_PROG_LN_S
AC_PROG_MAKE_SET
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
"x$enable_cust_verb" = "x"  
then

	#these don't make any sense, so we fall back to the default - INFO
	enable_cust_verb=INFO
	cust_verb_forced=no
fi

if test "x$enable_cust_verb" != "xDEBUG" -a "x$enable_cust_verb" != "xINFO" -a \
"x$enable_cust_verb" != "xWARNING" -a "x$enable_cust_verb" != "xERROR" -a \
"x$enable_cust_verb" != "xSEVERE_ERROR"  
then
	AC_MSG_ERROR([cust-verb should be on of DEBUG, INFO, WARNING, ERROR,
			SEVERE_ERROR])
fi

AC_DEFINE_UNQUOTED([HAVE_VERBOSITY_COMPILE], [EC_V_${enable_cust_verb}], [Compiled-in verbosity level])

# Checks for libraries.
AC_CHECK_LIB([readline], [readline], , AC_MSG_ERROR([You don't seem to have readline
			library]))

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h libintl.h limits.h malloc.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h strings.h sys/socket.h sys/time.h unistd.h execinfo.h termio.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_STRTOD
AC_CHECK_FUNCS([ftruncate gethostbyname gethostname gettimeofday memmove memset regcomp select socket strcasecmp strchr strdup strerror strstr strtol backtrace])


AC_ARG_ENABLE([tcmalloc], [AS_HELP_STRING([--enable-tcmalloc], [Enable linking with tcmalloc (brand new malloc from Google)])])
if test "x$enable_tcmalloc" = "xyes"  
then
	AC_CHECK_LIB([tcmalloc], [malloc], , AC_MSG_ERROR([You don't seem to have
			tcmalloc library!]))
fi

AC_CHECK_LIB([pthread], [pthread_create], , AC_MSG_ERROR([You don't seem to have
			pthread library!]))
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile src/Makefile src/Log/Makefile src/Lock/Makefile
		src/Errors/Makefile src/Store/Makefile src/ARIESLog/Makefile
		src/Indexes/Makefile 
		src/QueryParser/Makefile src/TransactionManager/Makefile
		src/SystemStats/Makefile src/QueryExecutor/Makefile
		src/Client/Makefile src/AdminParser/Makefile
		src/Server/Makefile src/TypeCheck/Makefile
		src/Config/Makefile src/Util/Makefile Documentation/Makefile
		src/Protocol/Makefile src/Protocol/Streams/Makefile
		src/Protocol/Enums/Makefile src/Protocol/Packages/Data/Makefile
		src/Protocol/Packages/Makefile src/CBindings/Makefile])
AC_OUTPUT

if test "x$cust_verb_forced" = "xno" ; then
	AC_MSG_WARN([Not all messages will be compiled, which is probably fine
		     for you (by default only debug messages are not compiled),
		     but if you would like to change this behaviour consult the
		     --enable-cust-verb option.])
fi

