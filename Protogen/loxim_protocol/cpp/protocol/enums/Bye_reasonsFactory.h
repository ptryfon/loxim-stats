#ifndef BYE_REASONSFACTORY_H_
#define BYE_REASONSFACTORY_H_

#include <stdint.h>

namespace protocol{

enum Bye_reasonsEnum {
	br_reason1
};

struct Bye_reasonsMap{
	unsigned br_reason1:1;
};
const Bye_reasonsMap BYE_REASONSMAP_ALL={1};
const Bye_reasonsMap BYE_REASONSMAP_NONE={0};
const Bye_reasonsMap BYE_REASONSMAP_ODD={1};

class Bye_reasonsFactory{
public:
	static bool getEnumByValue(uint32_t value,enum Bye_reasonsEnum& res);
	static bool getValueByEnum(enum Bye_reasonsEnum value,uint32_t& res);

	static bool getEnumMapByValue(uint32_t value,Bye_reasonsMap& res);
	static bool getValueByEnumMap(Bye_reasonsMap value,uint32_t& res);
	static bool mapEquals(Bye_reasonsMap val1,Bye_reasonsMap val2);
};//enum
};//namespace
#endif

