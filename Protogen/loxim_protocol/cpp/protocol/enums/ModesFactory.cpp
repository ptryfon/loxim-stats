#include "ModesFactory.h"

using namespace protocol;


bool ModesFactory::getEnumByValue(uint64_t value,enum ModesEnum& res){
	if ((value==1)){res= mode_ssl;return true;};
	if ((value==2)){res= mode_zlib;return true;};
	return false;
}

bool ModesFactory::getValueByEnum(enum ModesEnum value,uint64_t& res){
	switch (value){
		case mode_ssl: res= 1;return true;
		case mode_zlib: res= 2;return true;
		default: return false;
	};
}
bool ModesFactory::getEnumMapByValue(uint64_t value,ModesMap& res){
	res.mode_ssl=(((value&1)==1)?1:0);
	res.mode_zlib=(((value&2)==2)?1:0);
	return true;
}

bool ModesFactory::getValueByEnumMap(ModesMap value,uint64_t& res){
	res=0;
	if (value.mode_ssl) res|=1;
	if (value.mode_zlib) res|=2;
	return true;
};
bool ModesFactory::mapEquals(ModesMap val1,ModesMap val2){
	return (val1.mode_ssl==val2.mode_ssl)&&(val1.mode_zlib==val2.mode_zlib)&&(true);
};

