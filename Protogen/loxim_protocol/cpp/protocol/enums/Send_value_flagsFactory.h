#ifndef SEND_VALUE_FLAGSFACTORY_H_
#define SEND_VALUE_FLAGSFACTORY_H_

#include <stdint.h>

namespace protocol{

enum Send_value_flagsEnum {
	svf_to_be_continued
};

struct Send_value_flagsMap{
	unsigned svf_to_be_continued:1;
};
const Send_value_flagsMap SEND_VALUE_FLAGSMAP_ALL={1};
const Send_value_flagsMap SEND_VALUE_FLAGSMAP_NONE={0};
const Send_value_flagsMap SEND_VALUE_FLAGSMAP_ODD={1};

class Send_value_flagsFactory{
public:
	static bool getEnumByValue(uint8_t value,enum Send_value_flagsEnum& res);
	static bool getValueByEnum(enum Send_value_flagsEnum value,uint8_t& res);

	static bool getEnumMapByValue(uint8_t value,Send_value_flagsMap& res);
	static bool getValueByEnumMap(Send_value_flagsMap value,uint8_t& res);
	static bool mapEquals(Send_value_flagsMap val1,Send_value_flagsMap val2);
};//enum
};//namespace
#endif

