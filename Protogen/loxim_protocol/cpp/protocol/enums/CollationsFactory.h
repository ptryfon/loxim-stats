#ifndef COLLATIONSFACTORY_H_
#define COLLATIONSFACTORY_H_

#include <stdint.h>

namespace protocol{

enum CollationsEnum {
	coll_default
	,coll_unicode_collation_alghorithm_uts10
};

struct CollationsMap{
	unsigned coll_default:1;
	unsigned coll_unicode_collation_alghorithm_uts10:1;
};
const CollationsMap COLLATIONSMAP_ALL={1,1};
const CollationsMap COLLATIONSMAP_NONE={0,0};
const CollationsMap COLLATIONSMAP_ODD={1,0};

class CollationsFactory{
public:
	static bool getEnumByValue(uint64_t value,enum CollationsEnum& res);
	static bool getValueByEnum(enum CollationsEnum value,uint64_t& res);

	static bool getEnumMapByValue(uint64_t value,CollationsMap& res);
	static bool getValueByEnumMap(CollationsMap value,uint64_t& res);
	static bool mapEquals(CollationsMap val1,CollationsMap val2);
};//enum
};//namespace
#endif

