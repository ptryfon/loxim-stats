#ifndef FEATURESFACTORY_H_
#define FEATURESFACTORY_H_

#include <stdint.h>

namespace protocol{

enum FeaturesEnum {
	f_ssl
	,f_o_ssl
	,f_zlib
	,f_autocommit
	,f_optimization
};

struct FeaturesMap{
	unsigned f_ssl:1;
	unsigned f_o_ssl:1;
	unsigned f_zlib:1;
	unsigned f_autocommit:1;
	unsigned f_optimization:1;
};
const FeaturesMap FEATURESMAP_ALL={1,1,1,1,1};
const FeaturesMap FEATURESMAP_NONE={0,0,0,0,0};
const FeaturesMap FEATURESMAP_ODD={1,0,1,0,1};

class FeaturesFactory{
public:
	static bool getEnumByValue(uint64_t value,enum FeaturesEnum& res);
	static bool getValueByEnum(enum FeaturesEnum value,uint64_t& res);

	static bool getEnumMapByValue(uint64_t value,FeaturesMap& res);
	static bool getValueByEnumMap(FeaturesMap value,uint64_t& res);
	static bool mapEquals(FeaturesMap val1,FeaturesMap val2);
};//enum
};//namespace
#endif

