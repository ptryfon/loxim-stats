#include "Bye_reasonsFactory.h"

using namespace protocol;


bool Bye_reasonsFactory::getEnumByValue(uint32_t value,enum Bye_reasonsEnum& res){
	if ((value==0x0001)){res= br_reason1;return true;};
	return false;
}

bool Bye_reasonsFactory::getValueByEnum(enum Bye_reasonsEnum value,uint32_t& res){
	switch (value){
		case br_reason1: res= 0x0001;return true;
		default: return false;
	};
}
bool Bye_reasonsFactory::getEnumMapByValue(uint32_t value,Bye_reasonsMap& res){
	res.br_reason1=(((value&0x0001)==0x0001)?1:0);
	return true;
}

bool Bye_reasonsFactory::getValueByEnumMap(Bye_reasonsMap value,uint32_t& res){
	res=0;
	if (value.br_reason1) res|=0x0001;
	return true;
};
bool Bye_reasonsFactory::mapEquals(Bye_reasonsMap val1,Bye_reasonsMap val2){
	return (val1.br_reason1==val2.br_reason1)&&(true);
};

