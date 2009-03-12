
#include <string>

#include <stdlib.h>
#include <string.h>

#include <Client/SimpleClient.h>
#include <Protocol/Packages/Package.h>
#include <CBindings/loxim_c_client.h>



LXDATA* lx__convert_data(const Protocol::Package *, LXDATA *);
void lx__dispose_data(LXDATA *, bool);

void lx__init_error(struct lx_error *e)
{
	if (e == NULL)
		return;
	e->code = 0;
	e->msg = NULL;
}

void lx__set_error(struct lx_error *e, int code, const char *msg)
{
	if (e->msg != NULL)
		free(e->msg);
	e->code = code;
	e->msg = NULL;
	if (msg != NULL)
		e->msg = strdup(msg);
}

void lx__destroy_error(struct lx_error *e)
{
	lx__set_error(e, 0, NULL);
}

const char *liblx_version = Client::SimpleClient::version.c_str();

LXCONN lx_connect_p(const char *host, int port, const char *user, const char *password)
{
	LXCONN c = (LXCONN) malloc(sizeof(struct lx_conn));
	c->cli = NULL;
	lx__init_error(&c->err);
	try {
		Client::SimpleClient *scl;
		scl = new Client::SimpleClient(host, port, user, password);
		c->cli = scl;
		scl->connect();
	} catch (Client::ErrorException &e) {
		lx__set_error(&c->err, e.Code(), e.Msg().c_str());
	}
	return c;
}

LXCONN lx_connect_s(const char *params)
{
	LXCONN c = (LXCONN) malloc(sizeof(struct lx_conn));
	c->cli = NULL;
	lx__init_error(&c->err);
	try {
		Client::SimpleClient *scl;
		scl = new Client::SimpleClient(params);
		c->cli = scl;
		scl->connect();
	} catch (Client::ErrorException &e) {
		lx__set_error(&c->err, e.Code(), e.Msg().c_str());
	}
	return c;
}

LXRES lx_query(LXCONN c, const char *query)
{
	if (c == NULL)
		return NULL;
	Client::SimpleClient *scl;
	scl = (Client::SimpleClient*)c->cli;
	if (scl == NULL)
		return NULL;

	LXRES r = (LXRES) malloc(sizeof(struct lx_res));
	r->conn = c;
	r->data = NULL;
	lx__init_error(&r->err);
	try {
		const Protocol::Package *data;
		scl->execute(query, data);
		r->data = lx__convert_data(data, NULL);
	} catch (Client::ErrorException &e) {
		lx__set_error(&r->err, e.Code(), e.Msg().c_str());
	}
	return r;
}

LXRES lx_query_params(LXCONN c, const char *query, length_t nparams, const char * const *keys, const char * const *values, const length_t *lengths)
{
	if (c == NULL)
		return NULL;
	Client::SimpleClient *scl;
	scl = (Client::SimpleClient*)c->cli;
	if (scl == NULL)
		return NULL;

	LXRES r = (LXRES) malloc(sizeof(struct lx_res));
	r->conn = c;
	r->data = NULL;
	lx__init_error(&r->err);
	try {
		const Protocol::Package *data;
		scl->executeParams(query, data, nparams, keys, values,
			(const int*)lengths);
		r->data = lx__convert_data(data, NULL);
	} catch (Client::ErrorException &e) {
		lx__set_error(&r->err, e.Code(), e.Msg().c_str());
	}
	return r;
}

LXDATA* lx_fetch(LXRES r)
{
	return (r == NULL ? NULL : r->data);
}

void lx_close(LXCONN c)
{
	if (c == NULL)
		return;
	Client::SimpleClient *scl;
	scl = (Client::SimpleClient*)c->cli;
	if (scl == NULL)
		return;

	try {
		scl->disconnect();
	} catch (Client::ErrorException &e) {}

	delete scl;
	lx__destroy_error(&c->err);
	free(c);
}

void lx_dispose(LXRES r)
{
	if (r == NULL)
		return;

	if (r->data != NULL)
		lx__dispose_data(r->data, false);
	lx__destroy_error(&r->err);
	free(r);
}

LXERR lx_error_conn(LXCONN c)
{
	return (c == NULL ? NULL : &c->err);
}

LXERR lx_error_res(LXRES r)
{
	return (r == NULL ? NULL : &r->err);
}

int lx_status(LXCONN c, const char *code, char *buf, int maxlen)
{
	if (c == NULL)
		return 0;
	Client::SimpleClient *scl;
	scl = (Client::SimpleClient*)c->cli;
	if (scl == NULL)
		return 0;

	int res;
	if (false) {
	} else if (strcmp(code, "hostname") == 0) {
		strncpy(buf, scl->statusHostname().c_str(), maxlen);
		buf[maxlen-1] = '\0';
		return 1;
	} else if (strcmp(code, "hostaddr") == 0) {
		strncpy(buf, scl->statusHostaddr().c_str(), maxlen);
		buf[maxlen-1] = '\0';
		return 1;
	} else if (strcmp(code, "user") == 0) {
		strncpy(buf, scl->statusUser().c_str(), maxlen);
		buf[maxlen-1] = '\0';
		return 1;
	} else if (lx_status_int(c, code, &res)) {
		snprintf(buf, maxlen, "%d", res);
		return 1;
	} else
		return 0;
}

int lx_status_int(LXCONN c, const char *code, int *res)
{
	if (c == NULL)
		return 0;
	Client::SimpleClient *scl;
	scl = (Client::SimpleClient*)c->cli;
	if (scl == NULL)
		return 0;

	if (false) {
	} else if (strcmp(code, "connected") == 0) {
		*res = scl->statusConnected();
		return 1;
	} else if (strcmp(code, "time") == 0) {
		*res = scl->statusTime();
		return 1;
	} else if (strcmp(code, "port") == 0) {
		*res = scl->statusPort();
		return 1;
	} else
		return 0;
}

int lx_status_codes(char *buf, int maxlen)
{
	const char *codes[] = {
		"hostname", "hostaddr", "user",
		"connected", "time", "port" };

	char *cur = buf, *end = buf + maxlen;
	unsigned int i = 0, len;
	for (i = 0; i < sizeof(codes)/sizeof(codes[0]); i++) {
		len = strlen(codes[i]);
		if (cur + len + 1 >= end)
			break;
		strcpy(cur, codes[i]);
		cur += len;
		*cur = '\0';
		cur++;
	}
	*cur = '\0';
	return i < sizeof(codes)/sizeof(codes[0]) ? 0 : 1;
}

