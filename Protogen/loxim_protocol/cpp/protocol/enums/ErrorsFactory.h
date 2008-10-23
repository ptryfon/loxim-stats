#ifndef ERRORSFACTORY_H_
#define ERRORSFACTORY_H_

#include <stdint.h>

namespace protocol{

enum ErrorsEnum {
	error_internal
	,error_params_incomplete
	,error_no_such_value_id
	,error_operation_not_permited
};

struct ErrorsMap{
	unsigned error_internal:1;
	unsigned error_params_incomplete:1;
	unsigned error_no_such_value_id:1;
	unsigned error_operation_not_permited:1;
};
const ErrorsMap ERRORSMAP_ALL={1,1,1,1};
const ErrorsMap ERRORSMAP_NONE={0,0,0,0};
const ErrorsMap ERRORSMAP_ODD={1,0,1,0};

class ErrorsFactory{
public:
	static bool getEnumByValue(uint32_t value,enum ErrorsEnum& res);
	static bool getValueByEnum(enum ErrorsEnum value,uint32_t& res);

	static bool getEnumMapByValue(uint32_t value,ErrorsMap& res);
	static bool getValueByEnumMap(ErrorsMap value,uint32_t& res);
	static bool mapEquals(ErrorsMap val1,ErrorsMap val2);
};//enum
};//namespace
#endif

