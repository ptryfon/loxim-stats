#include "FeaturesFactory.h"

using namespace protocol;


bool FeaturesFactory::getEnumByValue(uint64_t value,enum FeaturesEnum& res){
	if ((value==0x0001)){res= f_ssl;return true;};
	if ((value==0x0002)){res= f_o_ssl;return true;};
	if ((value==0x0004)){res= f_zlib;return true;};
	if ((value==0x0010)){res= f_autocommit;return true;};
	if ((value==0x0020)){res= f_optimization;return true;};
	return false;
}

bool FeaturesFactory::getValueByEnum(enum FeaturesEnum value,uint64_t& res){
	switch (value){
		case f_ssl: res= 0x0001;return true;
		case f_o_ssl: res= 0x0002;return true;
		case f_zlib: res= 0x0004;return true;
		case f_autocommit: res= 0x0010;return true;
		case f_optimization: res= 0x0020;return true;
		default: return false;
	};
}
bool FeaturesFactory::getEnumMapByValue(uint64_t value,FeaturesMap& res){
	res.f_ssl=(((value&0x0001)==0x0001)?1:0);
	res.f_o_ssl=(((value&0x0002)==0x0002)?1:0);
	res.f_zlib=(((value&0x0004)==0x0004)?1:0);
	res.f_autocommit=(((value&0x0010)==0x0010)?1:0);
	res.f_optimization=(((value&0x0020)==0x0020)?1:0);
	return true;
}

bool FeaturesFactory::getValueByEnumMap(FeaturesMap value,uint64_t& res){
	res=0;
	if (value.f_ssl) res|=0x0001;
	if (value.f_o_ssl) res|=0x0002;
	if (value.f_zlib) res|=0x0004;
	if (value.f_autocommit) res|=0x0010;
	if (value.f_optimization) res|=0x0020;
	return true;
};
bool FeaturesFactory::mapEquals(FeaturesMap val1,FeaturesMap val2){
	return (val1.f_ssl==val2.f_ssl)&&(val1.f_o_ssl==val2.f_o_ssl)&&(val1.f_zlib==val2.f_zlib)&&(val1.f_autocommit==val2.f_autocommit)&&(val1.f_optimization==val2.f_optimization)&&(true);
};

