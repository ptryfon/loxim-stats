#ifndef __LIBLX_H__
#define __LIBLX_H__


extern const char *liblx_version;

typedef long long int LXREF;
typedef unsigned int length_t;

struct lx_error {
	int code;
	char *msg;
};

struct lx_conn {
	void *cli;
	struct lx_error err;
};

struct lx_data {
	int type;
	union {
		int intval;
		double doubleval;
		LXREF refval;
		struct {
			char *varcharval;
			int varcharlen;
		};
		struct {
			struct lx_data *array;
			int length;
		};
	} value;
};

struct lx_res {
	struct lx_conn *conn;
	struct lx_data *data;
	struct lx_error err;
};

typedef struct lx_conn* LXCONN;
typedef struct lx_res* LXRES;
typedef struct lx_data LXDATA;
typedef struct lx_error* LXERR;

#define LX_DATATYPE_NULL	0x000
#define	LX_DATATYPE_VOID	0x100
#define	LX_DATATYPE_VARCHAR	0x101
#define	LX_DATATYPE_INT		0x102
#define	LX_DATATYPE_DOUBLE	0x103
#define	LX_DATATYPE_BOOL	0x104
#define LX_DATATYPE_REF		0x105
#define LX_DATATYPE_BAG		0x200
#define LX_DATATYPE_STRUCT	0x201
#define LX_DATATYPE_SEQ		0x202
#define LX_DATATYPE_BIND	0x203
#define LX_DATA_SIMPLE(x) ((x) & 0x100)
#define LX_DATA_COMPLEX(x) ((x) & 0x200)

#ifdef __cplusplus
extern "C" {
#endif

int lx_get_type(LXDATA *d);
char* lx_get_varchar(LXDATA *d);
int lx_get_varchar_len(LXDATA *d);
int lx_get_int(LXDATA *d);
int lx_get_bool(LXDATA *d);
double lx_get_double(LXDATA *d);
LXDATA* lx_get_children(LXDATA *d);
int lx_get_length(LXDATA *d);
LXDATA* lx_get_bag(LXDATA *d);
LXDATA* lx_get_struct(LXDATA *d);
LXDATA* lx_get_seq(LXDATA *d);
LXREF lx_get_ref(LXDATA *d);
char* lx_get_bind_key(LXDATA *d);
int lx_get_bind_key_len(LXDATA *d);
LXDATA* lx_get_bind_val(LXDATA *d);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

LXCONN lx_connect_p(const char *host, int port, const char *user, const char *password);
LXCONN lx_connect_s(const char *params);
LXRES lx_query(LXCONN c, const char *query);
LXRES lx_query_params(LXCONN c, const char *query, length_t nparams, const char * const *keys, const char * const *values, const length_t *lengths);
LXDATA* lx_fetch(LXRES r);
void lx_close(LXCONN c);
void lx_dispose(LXRES r);
LXERR lx_error_conn(LXCONN c);
LXERR lx_error_res(LXRES r);
int lx_status(LXCONN c, const char *code, char *buf, int maxlen);
int lx_status_int(LXCONN c, const char *code, int *res);
int lx_status_codes(char *buf, int maxlen);

#define lx_error_code(e) ((e)->code)
#define lx_error_msg(e) ((e)->msg)

#ifdef __cplusplus
}
#endif


#endif
