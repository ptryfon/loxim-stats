dnl $Id$
dnl config.m4 for extension lxsbql

PHP_ARG_WITH(lxsbql, for lxsbql support,
[  --with-lxsbql=DIR             Include lxsbql support])

if test "$PHP_LXSBQL" != "no"; then
  # --with-lxsbql -> check with-path
  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/loxim_client.h"
  if test -r $PHP_LXSBQL/$SEARCH_FOR; then # path given as parameter
    LXSBQL_DIR=$PHP_LXSBQL
  else # search default path list
    AC_MSG_CHECKING([for lxsbql files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        LXSBQL_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$LXSBQL_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the lxsbql distribution])
  fi

  # --with-lxsbql -> add include path
  PHP_ADD_INCLUDE($LXSBQL_DIR/include)

  # --with-lxsbql -> check for lib and symbol presence
  LIBNAME=loxim_client
  LIBSYMBOL=lx_query

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LXSBQL_DIR/lib, LXSBQL_SHARED_LIBADD)
    AC_DEFINE(HAVE_LIBLX,1,[ ])
    PHP_ADD_LIBRARY("stdc++")
    PHP_ADD_LIBRARY("pthread")
    PHP_ADD_LIBRARY("readline")
  ],[
    AC_MSG_ERROR([wrong loxim_client lib version or lib not found])
  ],[
    -L$LXSBQL_DIR/lib -lstdc++ -pthread -lreadline
  ])

  PHP_SUBST(LXSBQL_SHARED_LIBADD)

  PHP_NEW_EXTENSION(lxsbql, lxsbql.c, $ext_shared)
fi
