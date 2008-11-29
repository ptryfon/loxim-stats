AC_INIT([loxim], [0.01], [loxim-devel@lists.sourceforge.net])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_DISABLE_SHARED
AC_PROG_LIBTOOL
AC_CHECK_LIB([readline], [readline], , AC_MSG_ERROR([You don't seem to have readline
			library]))
AC_CHECK_LIB([pthread], [pthread_create], , AC_MSG_ERROR([You don't seem to have
			pthread library!]))
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

