#include "Send_value_flagsFactory.h"

using namespace protocol;


bool Send_value_flagsFactory::getEnumByValue(uint8_t value,enum Send_value_flagsEnum& res){
	if ((value==0x01)){res= svf_to_be_continued;return true;};
	return false;
}

bool Send_value_flagsFactory::getValueByEnum(enum Send_value_flagsEnum value,uint8_t& res){
	switch (value){
		case svf_to_be_continued: res= 0x01;return true;
		default: return false;
	};
}
bool Send_value_flagsFactory::getEnumMapByValue(uint8_t value,Send_value_flagsMap& res){
	res.svf_to_be_continued=(((value&0x01)==0x01)?1:0);
	return true;
}

bool Send_value_flagsFactory::getValueByEnumMap(Send_value_flagsMap value,uint8_t& res){
	res=0;
	if (value.svf_to_be_continued) res|=0x01;
	return true;
};
bool Send_value_flagsFactory::mapEquals(Send_value_flagsMap val1,Send_value_flagsMap val2){
	return (val1.svf_to_be_continued==val2.svf_to_be_continued)&&(true);
};

