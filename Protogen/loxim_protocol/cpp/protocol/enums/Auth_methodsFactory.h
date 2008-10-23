#ifndef AUTH_METHODSFACTORY_H_
#define AUTH_METHODSFACTORY_H_

#include <stdint.h>

namespace protocol{

enum Auth_methodsEnum {
	am_trust
	,am_mysql5
};

struct Auth_methodsMap{
	unsigned am_trust:1;
	unsigned am_mysql5:1;
};
const Auth_methodsMap AUTH_METHODSMAP_ALL={1,1};
const Auth_methodsMap AUTH_METHODSMAP_NONE={0,0};
const Auth_methodsMap AUTH_METHODSMAP_ODD={1,0};

class Auth_methodsFactory{
public:
	static bool getEnumByValue(uint64_t value,enum Auth_methodsEnum& res);
	static bool getValueByEnum(enum Auth_methodsEnum value,uint64_t& res);

	static bool getEnumMapByValue(uint64_t value,Auth_methodsMap& res);
	static bool getValueByEnumMap(Auth_methodsMap value,uint64_t& res);
	static bool mapEquals(Auth_methodsMap val1,Auth_methodsMap val2);
};//enum
};//namespace
#endif

