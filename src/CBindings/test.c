#include <stdio.h>
#include <string.h>

#include <CBindings/loxim_client.h>


void print_data(LXDATA *d, int lev)
{
	int i;
	LXDATA *ch;

	for (i = 0; i < lev; i++) putchar(' ');

	switch (lx_get_type(d)) {
	case LX_DATATYPE_VOID:
		printf("<void %d>\n", lx_get_int(d));
		break;
	case LX_DATATYPE_INT:
		printf("<int %d>\n", lx_get_int(d));
		break;
	case LX_DATATYPE_BOOL:
		printf("<bool %d>\n", lx_get_bool(d));
		break;
	case LX_DATATYPE_DOUBLE:
		printf("<double %g>\n", lx_get_double(d));
		break;
	case LX_DATATYPE_VARCHAR:
		printf("<varchar %s>\n", lx_get_varchar(d));
		break;
	case LX_DATATYPE_REF:
		printf("<ref %lld>\n", lx_get_ref(d));
		break;
	case LX_DATATYPE_BAG:
		printf("<bag> {\n");
		ch = lx_get_bag(d);
		for (i = 0; i < lx_get_length(d); i++)
			print_data(&ch[i], lev+1);
		for (i = 0; i < lev; i++) putchar(' ');
		printf("}\n");
		break;
	case LX_DATATYPE_STRUCT:
		printf("<struct> {\n");
		ch = lx_get_struct(d);
		for (i = 0; i < lx_get_length(d); i++)
			print_data(&ch[i], lev+1);
		for (i = 0; i < lev; i++) putchar(' ');
		printf("}\n");
		break;
	case LX_DATATYPE_SEQ:
		printf("<seq> {\n");
		ch = lx_get_seq(d);
		for (i = 0; i < lx_get_length(d); i++)
			print_data(&ch[i], lev+1);
		for (i = 0; i < lev; i++) putchar(' ');
		printf("}\n");
		break;
	case LX_DATATYPE_BIND:
		printf("<bind> %s:\n", lx_get_bind_key(d));
		print_data(lx_get_bind_val(d), lev+1);
		break;
	default:
		printf("<unknown type>\n");
	}
}

void show_error(LXERR e, const char *desc)
{
	printf("error (%s) code %d (0x%x) msg: %s//\n",
		desc, lx_error_code(e), lx_error_code(e), lx_error_msg(e));
}

void execute(LXCONN c, const char *q)
{
	LXDATA *d;
	LXRES r;

	printf("Q: %s\n", q);

	r = lx_query(c, q);
	show_error(lx_error_res(r), "res");

	d = lx_fetch(r);
	if (d != NULL)
		print_data(d, 4);

	lx_dispose(r);
}

void params()
{
	char buf[20];
	int r;
	char *list[10];
	char *cur;
	int i, n;
	r = lx_status_codes(buf, sizeof(buf));
	printf("retval: %d; sizeof: %d\n", r, sizeof(buf));

	n = 0;
	cur = buf;
	while (*cur != '\0') {
		list[n++] = cur;
		cur += strlen(cur)+1;
	}
	printf("n: %d\n", n);
	for (i = 0; i < n; i++) {
		int res, s1, s2;
		char buf[20];
		buf[0] = '\0';
		printf("pos[%d]: %s//\n", i, list[i]);
		s1 = lx_status(NULL, list[i], buf, sizeof(buf));
		s2 = lx_status_int(NULL, list[i], &res);
		printf("s1: %d s2: %d res: %d buf: %s//\n", s1, s2, res, buf);
	}
	printf("END\n");
}

int main()
{
	LXCONN c;

	printf("liblx version: %s\n", liblx_version);
//params();
//return (1);

	c = lx_connect_p("localhost", 2000, "root", "");
	show_error(lx_error_conn(c), "conn");

	execute(c, "begin");
//execute(c, "(2 union 3) order by vc");
//execute(c, "6.28;");
	execute(c, "ref(pick);");
	execute(c, "pick;");
	execute(c, "deref(pick);");
	execute(c, "axe axe;");
	execute(c, "end");

	execute(c, "begin");
	execute(c, "2;");
	execute(c, "3.14;");
	execute(c, "\"napis\";");
	execute(c, "4 < 87;");
	execute(c, "7 union 6");
	execute(c, "(5 union 4) order by x");
	execute(c, "(9, 8)");
	execute(c, "(4 as x, 5 as y)");
	execute(c, "6 as c");
	execute(c, "(7 as x) as n");
	execute(c, "end");

	lx_close(c);

	return 0;
}
