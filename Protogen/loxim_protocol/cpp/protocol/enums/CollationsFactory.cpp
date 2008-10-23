#include "CollationsFactory.h"

using namespace protocol;


bool CollationsFactory::getEnumByValue(uint64_t value,enum CollationsEnum& res){
	if ((value==0)){res= coll_default;return true;};
	if ((value==1)){res= coll_unicode_collation_alghorithm_uts10;return true;};
	return false;
}

bool CollationsFactory::getValueByEnum(enum CollationsEnum value,uint64_t& res){
	switch (value){
		case coll_default: res= 0;return true;
		case coll_unicode_collation_alghorithm_uts10: res= 1;return true;
		default: return false;
	};
}
bool CollationsFactory::getEnumMapByValue(uint64_t value,CollationsMap& res){
	res.coll_default=(((value&0)==0)?1:0);
	res.coll_unicode_collation_alghorithm_uts10=(((value&1)==1)?1:0);
	return true;
}

bool CollationsFactory::getValueByEnumMap(CollationsMap value,uint64_t& res){
	res=0;
	if (value.coll_default) res|=0;
	if (value.coll_unicode_collation_alghorithm_uts10) res|=1;
	return true;
};
bool CollationsFactory::mapEquals(CollationsMap val1,CollationsMap val2){
	return (val1.coll_default==val2.coll_default)&&(val1.coll_unicode_collation_alghorithm_uts10==val2.coll_unicode_collation_alghorithm_uts10)&&(true);
};

