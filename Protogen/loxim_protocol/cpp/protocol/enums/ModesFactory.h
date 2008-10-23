#ifndef MODESFACTORY_H_
#define MODESFACTORY_H_

#include <stdint.h>

namespace protocol{

enum ModesEnum {
	mode_ssl
	,mode_zlib
};

struct ModesMap{
	unsigned mode_ssl:1;
	unsigned mode_zlib:1;
};
const ModesMap MODESMAP_ALL={1,1};
const ModesMap MODESMAP_NONE={0,0};
const ModesMap MODESMAP_ODD={1,0};

class ModesFactory{
public:
	static bool getEnumByValue(uint64_t value,enum ModesEnum& res);
	static bool getValueByEnum(enum ModesEnum value,uint64_t& res);

	static bool getEnumMapByValue(uint64_t value,ModesMap& res);
	static bool getValueByEnumMap(ModesMap value,uint64_t& res);
	static bool mapEquals(ModesMap val1,ModesMap val2);
};//enum
};//namespace
#endif

