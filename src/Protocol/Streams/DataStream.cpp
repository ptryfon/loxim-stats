#include <Protocol/Streams/DataStream.h>

namespace Protocol {
	size_t DataStream::get_varuint_size(VarUint n)
	{
		if (n.is_null() || n.get_val() < 250)
			return 1;
		if (n.get_val() < 0xFFFFU)
			return 3;
		if (n.get_val() < 0xFFFFFFFFUL)
			return 5;
		return 9;
	}

	DataStream::~DataStream()
	{
	}
}
