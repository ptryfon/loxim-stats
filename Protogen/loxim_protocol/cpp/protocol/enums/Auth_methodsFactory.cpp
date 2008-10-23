#include "Auth_methodsFactory.h"

using namespace protocol;


bool Auth_methodsFactory::getEnumByValue(uint64_t value,enum Auth_methodsEnum& res){
	if ((value==0x0001)){res= am_trust;return true;};
	if ((value==0x0002)){res= am_mysql5;return true;};
	return false;
}

bool Auth_methodsFactory::getValueByEnum(enum Auth_methodsEnum value,uint64_t& res){
	switch (value){
		case am_trust: res= 0x0001;return true;
		case am_mysql5: res= 0x0002;return true;
		default: return false;
	};
}
bool Auth_methodsFactory::getEnumMapByValue(uint64_t value,Auth_methodsMap& res){
	res.am_trust=(((value&0x0001)==0x0001)?1:0);
	res.am_mysql5=(((value&0x0002)==0x0002)?1:0);
	return true;
}

bool Auth_methodsFactory::getValueByEnumMap(Auth_methodsMap value,uint64_t& res){
	res=0;
	if (value.am_trust) res|=0x0001;
	if (value.am_mysql5) res|=0x0002;
	return true;
};
bool Auth_methodsFactory::mapEquals(Auth_methodsMap val1,Auth_methodsMap val2){
	return (val1.am_trust==val2.am_trust)&&(val1.am_mysql5==val2.am_mysql5)&&(true);
};

