/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header,v 1.16.2.1.2.1 2007/01/01 19:32:09 iliaa Exp $ */

#ifndef PHP_LXSBQL_H
#define PHP_LXSBQL_H

extern zend_module_entry lxsbql_module_entry;
#define phpext_lxsbql_ptr &lxsbql_module_entry

#ifdef PHP_WIN32
#define PHP_LXSBQL_API __declspec(dllexport)
#else
#define PHP_LXSBQL_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include <loxim_client.h>

typedef struct _php_lxsbql_conn {
	LXCONN lxconn;
} php_lxsbql_conn;

typedef struct _php_lxsbql_res {
	LXRES lxres;
} php_lxsbql_res;

typedef struct _php_lxsbql_ref {
	LXREF lxref;
} php_lxsbql_ref;

#define PHP_LXSBQL_CONN_NAME "Loxim connection"
#define PHP_LXSBQL_RES_NAME "Loxim result"
#define PHP_LXSBQL_REF_NAME "Loxim reference"

PHP_MINIT_FUNCTION(lxsbql);
PHP_MSHUTDOWN_FUNCTION(lxsbql);
PHP_RINIT_FUNCTION(lxsbql);
PHP_RSHUTDOWN_FUNCTION(lxsbql);
PHP_MINFO_FUNCTION(lxsbql);

PHP_FUNCTION(lx_connect);
PHP_FUNCTION(lx_connect_params);
PHP_FUNCTION(lx_query);
PHP_FUNCTION(lx_query_params);
PHP_FUNCTION(lx_fetch);
PHP_FUNCTION(lx_deref);
PHP_FUNCTION(lx_refid);
PHP_FUNCTION(lx_mkref);
PHP_FUNCTION(lx_close);
PHP_FUNCTION(lx_dispose);
PHP_FUNCTION(lx_error_conn);
PHP_FUNCTION(lx_error);
PHP_FUNCTION(lx_status);


ZEND_BEGIN_MODULE_GLOBALS(lxsbql)
//	long  global_value;
//	char *global_string;
	long default_conn;
	long last_error;
ZEND_END_MODULE_GLOBALS(lxsbql)


/* In every utility function you add that needs to use variables 
   in php_lxsbql_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as LXSBQL_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define LXSBQL_G(v) TSRMG(lxsbql_globals_id, zend_lxsbql_globals *, v)
#else
#define LXSBQL_G(v) (lxsbql_globals.v)
#endif

#endif	/* PHP_LXSBQL_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4 fdm=marker
 */
