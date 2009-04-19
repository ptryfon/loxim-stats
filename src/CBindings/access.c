#include <CBindings/loxim_client.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

int lx_get_type(LXDATA *d)
{
	return (d == NULL ? LX_DATATYPE_NULL : d->type);
}

char* lx_get_varchar(LXDATA *d)
{
	return (d != NULL && d->type != LX_DATATYPE_VARCHAR ?
		NULL : d->value.varcharval);
}

int lx_get_varchar_len(LXDATA *d)
{
	return (d != NULL && d->type != LX_DATATYPE_VARCHAR ?
		0 : d->value.varcharlen);
}

int lx_get_int(LXDATA *d)
{
	return (d != NULL && d->type != LX_DATATYPE_INT ?
		0 : d->value.intval);
}

double lx_get_double(LXDATA *d)
{
	return (d != NULL && d->type != LX_DATATYPE_DOUBLE ?
		0 : d->value.doubleval);
}

int lx_get_bool(LXDATA *d)
{
	return (d != NULL && d->type != LX_DATATYPE_BOOL ?
		0 : d->value.intval);
}

int lx_get_length(LXDATA *d)
{
	return (d == NULL || LX_DATA_SIMPLE(d->type) ? 0 : d->value.length);
}

LXDATA* lx_get_children(LXDATA *d)
{
	return (d == NULL || LX_DATA_SIMPLE(d->type) ? NULL : d->value.array);
}

LXDATA* lx_get_bag(LXDATA *d)
{
	return (d != NULL && d->type != LX_DATATYPE_BAG ?
		NULL : d->value.array);
}

LXDATA* lx_get_struct(LXDATA *d)
{
	return (d != NULL && d->type != LX_DATATYPE_STRUCT ?
		NULL : d->value.array);
}

LXDATA* lx_get_seq(LXDATA *d)
{
	return (d != NULL && d->type != LX_DATATYPE_SEQ ?
		NULL : d->value.array);
}

LXREF lx_get_ref(LXDATA *d)
{
	return (d != NULL && d->type != LX_DATATYPE_REF ?
		0 : d->value.refval);
}

char* lx_get_bind_key(LXDATA *d)
{
	return (d != NULL && d->type != LX_DATATYPE_BIND ?
		NULL : d->value.array[0].value.varcharval);
}

int lx_get_bind_key_len(LXDATA *d)
{
	return (d != NULL && d->type != LX_DATATYPE_BIND ?
		0 : d->value.array[0].value.varcharlen);
}

LXDATA* lx_get_bind_val(LXDATA *d)
{
	return (d != NULL && d->type != LX_DATATYPE_BIND ?
		NULL : &d->value.array[1]);
}

