#ifndef STATEMENT_FLAGSFACTORY_H_
#define STATEMENT_FLAGSFACTORY_H_

#include <stdint.h>

namespace protocol{

enum Statement_flagsEnum {
	sf_execute
	,sf_readonly
	,sf_prefer_dfs
	,sf_prefer_bsf
};

struct Statement_flagsMap{
	unsigned sf_execute:1;
	unsigned sf_readonly:1;
	unsigned sf_prefer_dfs:1;
	unsigned sf_prefer_bsf:1;
};
const Statement_flagsMap STATEMENT_FLAGSMAP_ALL={1,1,1,1};
const Statement_flagsMap STATEMENT_FLAGSMAP_NONE={0,0,0,0};
const Statement_flagsMap STATEMENT_FLAGSMAP_ODD={1,0,1,0};

class Statement_flagsFactory{
public:
	static bool getEnumByValue(uint64_t value,enum Statement_flagsEnum& res);
	static bool getValueByEnum(enum Statement_flagsEnum value,uint64_t& res);

	static bool getEnumMapByValue(uint64_t value,Statement_flagsMap& res);
	static bool getValueByEnumMap(Statement_flagsMap value,uint64_t& res);
	static bool mapEquals(Statement_flagsMap val1,Statement_flagsMap val2);
};//enum
};//namespace
#endif

