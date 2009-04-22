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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include <loxim_client.h>

#include "php_lxsbql.h"


#define LXSBQL_VERSION "1.00"

#define CHECK_DEFAULT_CONN(x) if ((x) == -1) { php_error_docref(NULL TSRMLS_CC, E_WARNING, "Loxim default connection does not exist"); RETURN_FALSE; }

#define LXSBQL_FETCH_DATA	0x01
#define LXSBQL_FETCH_STRUCT	0x02
#define LXSBQL_FETCH_ALL	0x03

ZEND_DECLARE_MODULE_GLOBALS(lxsbql)

/* True global resources - no need for thread safety here */
static int le_lxsbql_conn;
static int le_lxsbql_res;
static int le_lxsbql_ref;


/* {{{ lxsbql_functions[]
 *
 * Every user visible function must have an entry in lxsbql_functions[].
 */
zend_function_entry lxsbql_functions[] = {
	PHP_FE(lx_connect,	NULL)
	PHP_FE(lx_connect_params,	NULL)
	PHP_FE(lx_query,	NULL)
	PHP_FE(lx_query_params,	NULL)
	PHP_FE(lx_fetch,	NULL)
	PHP_FE(lx_deref,	NULL)
	PHP_FE(lx_refid,	NULL)
	PHP_FE(lx_mkref,	NULL)
	PHP_FE(lx_close,	NULL)
	PHP_FE(lx_dispose,	NULL)
	PHP_FE(lx_error_conn,	NULL)
	PHP_FE(lx_error,	NULL)
	PHP_FE(lx_status,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in lxsbql_functions[] */
};
/* }}} */

/* {{{ lxsbql_module_entry
 */
zend_module_entry lxsbql_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"lxsbql",
	lxsbql_functions,
	PHP_MINIT(lxsbql),
	PHP_MSHUTDOWN(lxsbql),
	PHP_RINIT(lxsbql),
	PHP_RSHUTDOWN(lxsbql),
	PHP_MINFO(lxsbql),
#if ZEND_MODULE_API_NO >= 20010901
	LXSBQL_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_LXSBQL
ZEND_GET_MODULE(lxsbql)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("lxsbql.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_lxsbql_globals, lxsbql_globals)
    STD_PHP_INI_ENTRY("lxsbql.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_lxsbql_globals, lxsbql_globals)
PHP_INI_END()
*/
/* }}} */

static void php_lxsbql_init_globals(zend_lxsbql_globals *lxsbql_globals)
{
//	lxsbql_globals->global_value = 0;
//	lxsbql_globals->global_string = NULL;
}

static void php_lxsbql_conn_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_lxsbql_conn *conn = (php_lxsbql_conn*)rsrc->ptr;

	if (conn == NULL)
		return;

//php_printf(" * dtor conn ptr: %p\n", conn->lxconn);
	lx_close(conn->lxconn);

	efree(conn);
}

static void php_lxsbql_res_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_lxsbql_res *res = (php_lxsbql_res*)rsrc->ptr;

	if (res == NULL)
		return;

//php_printf(" * dtor res ptr: %p\n", res->lxres);
	lx_dispose(res->lxres);

	efree(res);
}

static void php_lxsbql_ref_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_lxsbql_ref *ref = (php_lxsbql_ref*)rsrc->ptr;

	if (ref == NULL)
		return;

//php_printf(" * dtor ref val: %lld\n", ref->lxref);
	efree(ref);
}

static char php_lxsbql_status_codes_buf[1000];
static char* php_lxsbql_status_codes[100];
static int php_lxsbql_status_codes_count = 0;

static void php_lxsbql_init_status_codes()
{
	char *cur = php_lxsbql_status_codes_buf;
	int n = 0;
	lx_status_codes(php_lxsbql_status_codes_buf,
			sizeof(php_lxsbql_status_codes_buf));
	while (*cur != '\0') {
		php_lxsbql_status_codes[n++] = cur;
		cur += strlen(cur)+1;
	}
	php_lxsbql_status_codes_count = n;
}

static char php_lxsbql_version_buf[100];

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(lxsbql)
{
	le_lxsbql_conn = zend_register_list_destructors_ex(php_lxsbql_conn_dtor,
		NULL, PHP_LXSBQL_CONN_NAME, module_number);
	le_lxsbql_res = zend_register_list_destructors_ex(php_lxsbql_res_dtor,
		NULL, PHP_LXSBQL_RES_NAME, module_number);
	le_lxsbql_ref = zend_register_list_destructors_ex(php_lxsbql_ref_dtor,
		NULL, PHP_LXSBQL_REF_NAME, module_number);

	ZEND_INIT_MODULE_GLOBALS(lxsbql, php_lxsbql_init_globals, NULL);

	sprintf(php_lxsbql_version_buf,
		"%s (liblx %s)", LXSBQL_VERSION, liblx_version);
	REGISTER_STRING_CONSTANT("LXSBQL_VERSION", php_lxsbql_version_buf, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LXSBQL_FETCH_DATA", LXSBQL_FETCH_DATA, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LXSBQL_FETCH_STRUCT", LXSBQL_FETCH_STRUCT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LXSBQL_FETCH_ALL", LXSBQL_FETCH_ALL, CONST_CS|CONST_PERSISTENT);

	php_lxsbql_init_status_codes();

	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(lxsbql)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(lxsbql)
{
	LXSBQL_G(default_conn) = -1;
	LXSBQL_G(last_error) = -1;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(lxsbql)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(lxsbql)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "lxsbql support", "enabled");
	php_info_print_table_row(2, "module version", LXSBQL_VERSION);
	php_info_print_table_row(2, "liblx version", liblx_version);
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

static void php_lxsbql_set_default_conn(long c TSRMLS_DC)
{
	LXSBQL_G(default_conn) = c;
}

static void php_lxsbql_set_last_error(long c TSRMLS_DC)
{
//php_printf(" +++ SLE %ld -> %ld\n", LXSBQL_G(last_error), c);
	if (c != -1)
		zend_list_addref(c);

	if (LXSBQL_G(last_error) != -1)
		zend_list_delete(LXSBQL_G(last_error));

	LXSBQL_G(last_error) = c;
}

static char* php_lxsbql_print_error(LXERR lxerr)
{
	char *msg;
	if (lx_error_code(lxerr) == 0)
		msg = estrdup("");
	else
		spprintf(&msg, 0, "[0x%x] %s",
			lx_error_code(lxerr), lx_error_msg(lxerr));
	return msg;
}

static zval* php_lxsbql_convert_data(LXDATA *d, int howto)
{
	zval *z = NULL;
	php_lxsbql_ref *ref;
	LXDATA *ch = NULL;
	zval *zc = NULL;
	int i, len;
	char *type = NULL;

	ALLOC_INIT_ZVAL(z);
	ZVAL_NULL(z);

	if (LX_DATA_SIMPLE(lx_get_type(d)) && !(howto & LXSBQL_FETCH_DATA))
		return NULL;

	switch (lx_get_type(d)) {
	case LX_DATATYPE_NULL:
	case LX_DATATYPE_VOID:
		ZVAL_NULL(z);
		break;
	case LX_DATATYPE_INT:
		ZVAL_LONG(z, lx_get_int(d));
		break;
	case LX_DATATYPE_BOOL:
		ZVAL_BOOL(z, lx_get_bool(d));
		break;
	case LX_DATATYPE_DOUBLE:
		ZVAL_DOUBLE(z, lx_get_double(d));
		break;
	case LX_DATATYPE_VARCHAR:
		ZVAL_STRINGL(z, lx_get_varchar(d), lx_get_varchar_len(d), 1);
		break;
	case LX_DATATYPE_REF:
		ref = emalloc(sizeof(php_lxsbql_ref));
		ref->lxref = lx_get_ref(d);
		ZEND_REGISTER_RESOURCE(z, ref, le_lxsbql_ref);
		break;
	case LX_DATATYPE_BIND:
		zval_ptr_dtor(&z);
		z = php_lxsbql_convert_data(lx_get_bind_val(d), howto);
		break;
	case LX_DATATYPE_BAG:
	case LX_DATATYPE_STRUCT:
	case LX_DATATYPE_SEQ:
		array_init(z);
		ch = lx_get_children(d);
		if (ch == NULL)
			break;
		len = lx_get_length(d);
		type = "unknown";
		if (lx_get_type(d) == LX_DATATYPE_BAG)
			type = "bag";
		if (lx_get_type(d) == LX_DATATYPE_STRUCT)
			type = "struct";
		if (lx_get_type(d) == LX_DATATYPE_SEQ)
			type = "seq";
		if (howto & LXSBQL_FETCH_STRUCT)
			add_assoc_string(z, "_type", type, 1);
		for (i = 0; i < len; i++)
			if (lx_get_type(&ch[i]) != LX_DATATYPE_BIND) {
				zc = php_lxsbql_convert_data(&ch[i], howto);
				if (zc != NULL)
					add_next_index_zval(z, zc);
			} else {
				zc = php_lxsbql_convert_data(lx_get_bind_val(&ch[i]), howto);
				if (zc != NULL)
					add_assoc_zval(z, lx_get_bind_key(&ch[i]), zc);
			}
		break;
	default:
		php_error(E_WARNING, "lx_fetch: unknown data type");
	}
	return z;
}

static int php_lxsbql_array_to_params(HashTable *arr, char ***keys, char ***values, length_t **lengths)
{
	int n = 0, i;
	HashPosition pointer;
	zval **data, temp;
	char *key;
	uint key_len;
	ulong index;

	if (arr == NULL)
		return 0;
	for (zend_hash_internal_pointer_reset_ex(arr, &pointer); zend_hash_get_current_data_ex(arr, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr, &pointer))
		if (zend_hash_get_current_key_ex(arr, &key, &key_len, &index, 0, &pointer) == HASH_KEY_IS_STRING)
			n++;
	if (n == 0)
		return 0;

	*keys = emalloc((n+1) * sizeof(char*));
	*values = emalloc((n+1) * sizeof(char*));
	*lengths = emalloc((n+1) * sizeof(length_t));
	i = 0;
	for (zend_hash_internal_pointer_reset_ex(arr, &pointer); zend_hash_get_current_data_ex(arr, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr, &pointer)) {
		if (zend_hash_get_current_key_ex(arr, &key, &key_len, &index, 0, &pointer) != HASH_KEY_IS_STRING)
			continue;

		temp = **data;
		zval_copy_ctor(&temp);
		convert_to_string(&temp);
		(*keys)[i] = estrndup(key, key_len+1);
		(*values)[i] = estrndup(Z_STRVAL(temp), Z_STRLEN(temp));
		(*lengths)[i] = Z_STRLEN(temp);
//php_printf(" + key: %s value: %s//\n", key, Z_STRVAL(temp));
		zval_dtor(&temp);
		i++;
	}
	(*keys)[i] = NULL;
	(*values)[i] = NULL;
	(*lengths)[i] = 0;
//php_printf("cnv: count: %d/%d\n", i, n);

	return i;
}

void php_lxsbql_free_params(int len, char **keys, char **values, length_t *lengths)
{
	int i;

	if (len <= 0)
		return;
	for (i = 0; i < len; i++) {
		efree(keys[i]);
		efree(values[i]);
	}
	efree(keys);
	efree(values);
	efree(lengths);
}

/* {{{ proto resource lx_connect(string params)
    */
PHP_FUNCTION(lx_connect)
{
	php_lxsbql_conn *conn;
	char *params = NULL;
	int argc = ZEND_NUM_ARGS();
	int params_len;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &params, &params_len) == FAILURE) 
		return;

	conn = emalloc(sizeof(php_lxsbql_conn));
	conn->lxconn = lx_connect_s(params);

	ZEND_REGISTER_RESOURCE(return_value, conn, le_lxsbql_conn);
//php_printf("connect: created resource %ld\n", Z_RESVAL_P(return_value));

	if (lx_error_code(lx_error_conn(conn->lxconn)) != 0) {
		php_error(E_WARNING, "lx_connect: failed to connect");
		php_lxsbql_set_last_error(Z_RESVAL_P(return_value) TSRMLS_CC);
		zend_list_delete(Z_RESVAL_P(return_value));
		RETURN_FALSE;
	}

	php_lxsbql_set_default_conn(Z_RESVAL_P(return_value) TSRMLS_CC);
	php_lxsbql_set_last_error(-1 TSRMLS_CC);
}
/* }}} */

/* {{{ proto resource lx_connect_params(string host, int port, string user, string password)
    */
PHP_FUNCTION(lx_connect_params)
{
	php_lxsbql_conn *conn;
	char *host = NULL;
	char *user = NULL;
	char *password = NULL;
	int argc = ZEND_NUM_ARGS();
	int host_len;
	int user_len;
	int password_len;
	long port;

	if (zend_parse_parameters(argc TSRMLS_CC, "slss", &host, &host_len, &port, &user, &user_len, &password, &password_len) == FAILURE) 
		RETURN_FALSE;

	conn = emalloc(sizeof(php_lxsbql_conn));
	conn->lxconn = lx_connect_p(host, port, user, password);

	ZEND_REGISTER_RESOURCE(return_value, conn, le_lxsbql_conn);
//php_printf("connect: created resource %ld\n", Z_RESVAL_P(return_value));

	if (lx_error_code(lx_error_conn(conn->lxconn)) != 0) {
		php_error(E_WARNING, "lx_connect_params: failed to connect");
		php_lxsbql_set_last_error(Z_RESVAL_P(return_value) TSRMLS_CC);
		zend_list_delete(Z_RESVAL_P(return_value));
		RETURN_FALSE;
	}

	php_lxsbql_set_default_conn(Z_RESVAL_P(return_value) TSRMLS_CC);
	php_lxsbql_set_last_error(-1 TSRMLS_CC);
}
/* }}} */

/* {{{ proto resource lx_query(string q [, resource conn])
    */
PHP_FUNCTION(lx_query)
{
	php_lxsbql_res *res;
	int argc = ZEND_NUM_ARGS();
	char *q = NULL;
	int q_len;
	int conn_id = -1;
	zval *zconn = NULL;
	php_lxsbql_conn *conn = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "s|r", &q, &q_len, &zconn) == FAILURE) 
		RETURN_FALSE;
//php_printf("query: default conn: %ld\n", LXSBQL_G(default_conn));
	if (zconn == NULL) {
		conn_id = LXSBQL_G(default_conn);
		CHECK_DEFAULT_CONN(conn_id);
	}

	ZEND_FETCH_RESOURCE(conn, php_lxsbql_conn*, &zconn, conn_id,
		PHP_LXSBQL_CONN_NAME, le_lxsbql_conn);

//php_printf("query: ptr: %p id: %ld Q:\n", conn->lxconn, zconn ? Z_RESVAL_P(zconn) : conn_id);
//PHPWRITE(q, q_len);
//php_printf("//\n");
	res = emalloc(sizeof(php_lxsbql_res));
	res->lxres = lx_query(conn->lxconn, q);

	ZEND_REGISTER_RESOURCE(return_value, res, le_lxsbql_res);
//php_printf("query: created resource %ld\n", Z_RESVAL_P(return_value));
}
/* }}} */

/* {{{ proto resource lx_query_params(string q [, array params [, resource conn]])
    */
PHP_FUNCTION(lx_query_params)
{
	php_lxsbql_res *res;
	int argc = ZEND_NUM_ARGS();
	char *q = NULL;
	int q_len;
	int conn_id = -1;
	zval *zparams = NULL;
	HashTable *params = NULL;
	zval *zconn = NULL;
	php_lxsbql_conn *conn = NULL;
	char **p_keys = NULL, **p_values = NULL;
	length_t *p_lengths = NULL;
	int nparams;

	if (zend_parse_parameters(argc TSRMLS_CC, "s|ar", &q, &q_len, &zparams, &zconn) == FAILURE) 
		RETURN_FALSE;
//php_printf("query_p: default conn: %ld\n", LXSBQL_G(default_conn));
	if (zconn == NULL) {
		conn_id = LXSBQL_G(default_conn);
		CHECK_DEFAULT_CONN(conn_id);
	}

	ZEND_FETCH_RESOURCE(conn, php_lxsbql_conn*, &zconn, conn_id,
		PHP_LXSBQL_CONN_NAME, le_lxsbql_conn);

	if (zparams != NULL)
		params = Z_ARRVAL_P(zparams);

//php_printf("query_p: ptr: %p id: %ld Q:\n", conn->lxconn, zconn ? Z_RESVAL_P(zconn) : conn_id);
//PHPWRITE(q, q_len);
//php_printf("//\n");
	nparams = php_lxsbql_array_to_params(params, &p_keys, &p_values, &p_lengths);
	res = emalloc(sizeof(php_lxsbql_res));
	res->lxres = lx_query_params(conn->lxconn, q, nparams, (const char**)p_keys, (const char**)p_values, p_lengths);
//for (i = 0; i < nparams; i++)
//printf("param %i: %s %s %d//\n", i, p_keys[i], p_values[i], p_lengths[i]);
	php_lxsbql_free_params(nparams, p_keys, p_values, p_lengths);

	ZEND_REGISTER_RESOURCE(return_value, res, le_lxsbql_res);
//php_printf("query_p: created resource %ld\n", Z_RESVAL_P(return_value));
}
/* }}} */

/* {{{ proto mixed lx_fetch(resource res [, int howto])
    */
PHP_FUNCTION(lx_fetch)
{
	int argc = ZEND_NUM_ARGS();
	int res_id = -1;
	zval *zres = NULL;
	php_lxsbql_res *res = NULL;
	zval *z = NULL;
	long howto = LXSBQL_FETCH_DATA;

	if (zend_parse_parameters(argc TSRMLS_CC, "r|l", &zres, &howto) == FAILURE) 
		RETURN_FALSE;

	ZEND_FETCH_RESOURCE(res, php_lxsbql_res*, &zres, res_id,
		PHP_LXSBQL_RES_NAME, le_lxsbql_res);
//php_printf("fetch: ptr: %p id: %ld\n", res->lxres, Z_RESVAL_P(zres));

	z = php_lxsbql_convert_data(lx_fetch(res->lxres), howto);
	if (z) {
		*return_value = *z;
		zval_copy_ctor(return_value);
		zval_ptr_dtor(&z);
	}
}
/* }}} */

/* {{{ proto mixed lx_deref(resource res [, int howto [, resource conn]])
    */
PHP_FUNCTION(lx_deref)
{
	int argc = ZEND_NUM_ARGS();
	int ref_id = -1;
	zval *zref = NULL;
	php_lxsbql_ref *ref = NULL;
	char *query;
	long howto = LXSBQL_FETCH_DATA;
	int conn_id = -1;
	zval *zconn = NULL;
	php_lxsbql_conn *conn = NULL;
	LXRES lxres = NULL;
	zval *z = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "r|lr", &zref, &howto, &zconn) == FAILURE) 
		RETURN_FALSE;

	ZEND_FETCH_RESOURCE(ref, php_lxsbql_ref*, &zref, ref_id,
		PHP_LXSBQL_REF_NAME, le_lxsbql_ref);
//php_printf("deref: val: %lld id: %ld\n", ref->lxref, Z_RESVAL_P(zref));

//php_printf("deref: default conn: %ld\n", LXSBQL_G(default_conn));
	if (zconn == NULL) {
		conn_id = LXSBQL_G(default_conn);
		CHECK_DEFAULT_CONN(conn_id);
	}

	ZEND_FETCH_RESOURCE(conn, php_lxsbql_conn*, &zconn, conn_id,
		PHP_LXSBQL_CONN_NAME, le_lxsbql_conn);

	spprintf(&query, 0, "deref(refid(%lld))", ref->lxref);
	lxres = lx_query(conn->lxconn, query);
	efree(query);

	if (lxres == NULL || lx_error_code(lx_error_res(lxres)) != 0)
		RETURN_NULL();
	z = php_lxsbql_convert_data(lx_fetch(lxres), howto);
	lx_dispose(lxres);

	if (z) {
		*return_value = *z;
		zval_copy_ctor(return_value);
		zval_ptr_dtor(&z);
	}
}
/* }}} */

/* {{{ proto string lx_refid(resource res)
    */
PHP_FUNCTION(lx_refid)
{
	int argc = ZEND_NUM_ARGS();
	int ref_id = -1;
	zval *zref = NULL;
	php_lxsbql_ref *ref = NULL;
	char *buf;

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zref) == FAILURE) 
		RETURN_FALSE;

	ZEND_FETCH_RESOURCE(ref, php_lxsbql_ref*, &zref, ref_id,
		PHP_LXSBQL_REF_NAME, le_lxsbql_ref);
//php_printf("refid: val: %lld id: %ld\n", ref->lxref, Z_RESVAL_P(zref));

	spprintf(&buf, 0, "%lld", ref->lxref);
	RETURN_STRING(buf, 0);
}
/* }}} */

/* {{{ proto resource lx_mkref(mixed id)
    */
PHP_FUNCTION(lx_mkref)
{
	int argc = ZEND_NUM_ARGS();
	zval *zid;
	php_lxsbql_ref *ref = NULL;
	LXREF refval = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "z", &zid) == FAILURE) 
		RETURN_FALSE;
	if (Z_TYPE_P(zid) == IS_LONG)
		refval = Z_LVAL_P(zid);
	if (Z_TYPE_P(zid) == IS_STRING)
		refval = strtoull(Z_STRVAL_P(zid), NULL, 10);
	if (refval == 0)
		RETURN_FALSE;

	ref = emalloc(sizeof(php_lxsbql_ref));
	ref->lxref = refval;
	ZEND_REGISTER_RESOURCE(return_value, ref, le_lxsbql_ref);
}
/* }}} */

/* {{{ proto void lx_close([resource conn])
    */
PHP_FUNCTION(lx_close)
{
	int argc = ZEND_NUM_ARGS();
	int conn_id = -1;
	zval *zconn = NULL;
	php_lxsbql_conn *conn = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "|r", &zconn) == FAILURE) 
		RETURN_FALSE;
//php_printf("close: default conn: %ld\n", LXSBQL_G(default_conn));
	if (zconn == NULL) {
		conn_id = LXSBQL_G(default_conn);
		CHECK_DEFAULT_CONN(conn_id);
	}

	ZEND_FETCH_RESOURCE(conn, php_lxsbql_conn*, &zconn, conn_id,
		PHP_LXSBQL_CONN_NAME, le_lxsbql_conn);

//php_printf("close: ptr: %p id: %ld\n", conn->lxconn, zconn ? Z_RESVAL_P(zconn) : conn_id);
	if (zconn != NULL)
		conn_id = Z_RESVAL_P(zconn);
	zend_list_delete(conn_id);

	if (conn_id == LXSBQL_G(default_conn))
		php_lxsbql_set_default_conn(-1 TSRMLS_CC);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void lx_dispose(resource res)
    */
PHP_FUNCTION(lx_dispose)
{
	int argc = ZEND_NUM_ARGS();
	int res_id = -1;
	zval *zres = NULL;
	php_lxsbql_res *res = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zres) == FAILURE) 
		RETURN_FALSE;

	ZEND_FETCH_RESOURCE(res, php_lxsbql_res*, &zres, res_id,
		PHP_LXSBQL_RES_NAME, le_lxsbql_res);
//php_printf("dispose: ptr: %p id: %ld\n", res->lxres, Z_RESVAL_P(zres));

	zend_list_delete(Z_RESVAL_P(zres));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string lx_error_conn([resource conn])
    */
PHP_FUNCTION(lx_error_conn)
{
	int argc = ZEND_NUM_ARGS();
	int conn_id = -1;
	zval *zconn = NULL;
	php_lxsbql_conn *conn = NULL;
	char *msg = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "|r", &zconn) == FAILURE) 
		RETURN_FALSE;
//php_printf("error_con: default conn: %ld\n", LXSBQL_G(default_conn));
	if (zconn == NULL) {
		conn_id = LXSBQL_G(last_error);
		if (conn_id == -1)
			conn_id = LXSBQL_G(default_conn);
		CHECK_DEFAULT_CONN(conn_id);
	}

	ZEND_FETCH_RESOURCE(conn, php_lxsbql_conn*, &zconn, conn_id,
		PHP_LXSBQL_CONN_NAME, le_lxsbql_conn);

	msg = php_lxsbql_print_error(lx_error_conn(conn->lxconn));
	RETURN_STRING(msg, 0);
}
/* }}} */

/* {{{ proto string lx_error(resource res)
    */
PHP_FUNCTION(lx_error)
{
	int argc = ZEND_NUM_ARGS();
	int res_id = -1;
	zval *zres = NULL;
	php_lxsbql_res *res = NULL;
	char *msg = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zres) == FAILURE) 
		RETURN_FALSE;

	ZEND_FETCH_RESOURCE(res, php_lxsbql_res*, &zres, res_id,
		PHP_LXSBQL_RES_NAME, le_lxsbql_res);
//php_printf("error_res: ptr: %p id: %ld\n", res->lxres, Z_RESVAL_P(zres));

	msg = php_lxsbql_print_error(lx_error_res(res->lxres));
	RETURN_STRING(msg, 0);
}
/* }}} */

/* {{{ proto array lx_status([resource conn])
    */
PHP_FUNCTION(lx_status)
{
	int argc = ZEND_NUM_ARGS();
	int conn_id = -1;
	zval *zconn = NULL;
	php_lxsbql_conn *conn = NULL;
	zval *arr;
	unsigned int i;
	int res;
	char buf[1000];

	if (zend_parse_parameters(argc TSRMLS_CC, "|r", &zconn) == FAILURE) 
		RETURN_FALSE;
//php_printf("status: default conn: %ld\n", LXSBQL_G(default_conn));
	if (zconn == NULL) {
		conn_id = LXSBQL_G(default_conn);
		CHECK_DEFAULT_CONN(conn_id);
	}

	ZEND_FETCH_RESOURCE(conn, php_lxsbql_conn*, &zconn, conn_id,
		PHP_LXSBQL_CONN_NAME, le_lxsbql_conn);
//php_printf("status: ptr: %p id: %ld\n", conn->lxconn, Z_RESVAL_P(zconn));

	ALLOC_INIT_ZVAL(arr);
	array_init(arr);
	for (i = 0; i < php_lxsbql_status_codes_count; i++) {
		if (lx_status_int(conn->lxconn, php_lxsbql_status_codes[i], &res))
			add_assoc_long(arr, php_lxsbql_status_codes[i], res);
		else if (lx_status(conn->lxconn, php_lxsbql_status_codes[i], buf, sizeof(buf)))
			add_assoc_string(arr, php_lxsbql_status_codes[i], buf, 1);
		else
			add_assoc_null(arr, php_lxsbql_status_codes[i]);
	}
	*return_value = *arr;
	zval_copy_ctor(return_value);
	zval_ptr_dtor(&arr);
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4 fdm=marker
 */
