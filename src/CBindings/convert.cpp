
#include <cstring>
#include <cstdlib>

#include <Protocol/Enums/DataPackages.h>
#include <Protocol/Packages/Data/VoidPackage.h>
#include <Protocol/Packages/Data/Sint32Package.h>
#include <Protocol/Packages/Data/BoolPackage.h>
#include <Protocol/Packages/Data/VarcharPackage.h>
#include <Protocol/Packages/Data/DoublePackage.h>
#include <Protocol/Packages/Data/BagPackage.h>
#include <Protocol/Packages/Data/SequencePackage.h>
#include <Protocol/Packages/Data/StructPackage.h>
#include <Protocol/Packages/Data/RefPackage.h>
#include <Protocol/Packages/Data/BindingPackage.h>

#include <CBindings/loxim_c_client.h>

char* alloc_str(const char *begin, size_t length)
{
	char *tmp = (char*)malloc(length+1);
	memcpy(tmp, begin, length);
	tmp[length] = '\0';
	return tmp;
}

LXDATA* lx__convert_data(const Protocol::Package *in, LXDATA *prealloc)
{
	if (in == NULL)
		return NULL;

	LXDATA *out;
	if (prealloc == NULL)
		out = (LXDATA*) malloc(sizeof(struct lx_data));
	else
		out = prealloc;

	int i, size;
	const Protocol::ByteBuffer *bb;
	switch (in->get_type()) {
	case Protocol::VOID_PACKAGE:
		out->type = LX_DATATYPE_VOID;
		out->value.intval = 0;
		break;
	case Protocol::SINT32_PACKAGE:
		out->type = LX_DATATYPE_INT;
		out->value.intval =
			((Protocol::Sint32Package*)in)->get_val_value();
		break;
	case Protocol::BOOL_PACKAGE:
		out->type = LX_DATATYPE_BOOL;
		out->value.intval =
			((Protocol::BoolPackage*)in)->get_val_value();
		break;
	case Protocol::DOUBLE_PACKAGE:
		out->type = LX_DATATYPE_DOUBLE;
		out->value.doubleval =
			((Protocol::DoublePackage*)in)->get_val_value();
		break;
	case Protocol::VARCHAR_PACKAGE:
		out->type = LX_DATATYPE_VARCHAR;
		bb = &((Protocol::VarcharPackage*)in)->get_val_value();
		out->value.varcharval =
			alloc_str(bb->get_const_data(), bb->get_size());
		out->value.varcharlen = bb->get_size();
		break;
	case Protocol::REF_PACKAGE:
		out->type = LX_DATATYPE_REF;
		out->value.array = (LXDATA*) malloc(sizeof(struct lx_data));
		out->value.refval = ((Protocol::RefPackage*)in)->get_val_value_id();
		break;
	case Protocol::BAG_PACKAGE:
		out->type = LX_DATATYPE_BAG;
		size = ((Protocol::BagPackage*)in)->get_packages().size();
		out->value.length = size;
		out->value.array = (LXDATA*) malloc(size * sizeof(struct lx_data));
		for (i = 0; i < size; i++)
			lx__convert_data(((Protocol::BagPackage*)in)->get_packages()[i].get(), &out->value.array[i]);
		break;
	case Protocol::STRUCT_PACKAGE:
		out->type = LX_DATATYPE_STRUCT;
		size = ((Protocol::StructPackage*)in)->get_packages().size();
		out->value.length = size;
		out->value.array = (LXDATA*) malloc(size * sizeof(struct lx_data));
		for (i = 0; i < size; i++)
			lx__convert_data(((Protocol::StructPackage*)in)->get_packages()[i].get(), &out->value.array[i]);
		break;
	case Protocol::SEQUENCE_PACKAGE:
		out->type = LX_DATATYPE_SEQ;
		size = ((Protocol::SequencePackage*)in)->get_packages().size();
		out->value.length = size;
		out->value.array = (LXDATA*) malloc(size * sizeof(struct lx_data));
		for (i = 0; i < size; i++)
			lx__convert_data(((Protocol::SequencePackage*)in)->get_packages()[i].get(), &out->value.array[i]);
		break;
	case Protocol::BINDING_PACKAGE:
		out->type = LX_DATATYPE_BIND;
		out->value.length = 2;
		out->value.array = (LXDATA*) malloc(2 * sizeof(struct lx_data));
		bb = &((Protocol::BindingPackage*)in)->get_val_binding_name();
		out->value.array[0].type = LX_DATATYPE_VARCHAR;
		out->value.array[0].value.varcharval =
			alloc_str(bb->get_const_data(), bb->get_size());
		out->value.array[0].value.varcharlen = bb->get_size();
		lx__convert_data(&((Protocol::BindingPackage*)in)->get_val_value(), &out->value.array[1]);
		break;
	default:
		out->type = LX_DATATYPE_VOID;
		out->value.intval = -1;
	}
	return out;
}

void lx__dispose_data(LXDATA *d, bool prealloc)
{
	int i;
	if (LX_DATA_COMPLEX(d->type)) {
		for (i = 0; i < d->value.length; i++)
			lx__dispose_data(&d->value.array[i], true);
		free(d->value.array);
	}
	if (d->type == LX_DATATYPE_VARCHAR)
		free(d->value.varcharval);
	if (!prealloc)
		free(d);
}

