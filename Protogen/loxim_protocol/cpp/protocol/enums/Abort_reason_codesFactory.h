#ifndef ABORT_REASON_CODESFACTORY_H_
#define ABORT_REASON_CODESFACTORY_H_

#include <stdint.h>

namespace protocol{

enum Abort_reason_codesEnum {
	arc_administration_reason
	,arc_transaction_victim
	,arc_operation_not_permited
	,arc_time_limit_exceeded
	,arc_out_of_memory
	,arc_type_check_error
	,arc_other_run_time_error
};

struct Abort_reason_codesMap{
	unsigned arc_administration_reason:1;
	unsigned arc_transaction_victim:1;
	unsigned arc_operation_not_permited:1;
	unsigned arc_time_limit_exceeded:1;
	unsigned arc_out_of_memory:1;
	unsigned arc_type_check_error:1;
	unsigned arc_other_run_time_error:1;
};
const Abort_reason_codesMap ABORT_REASON_CODESMAP_ALL={1,1,1,1,1,1,1};
const Abort_reason_codesMap ABORT_REASON_CODESMAP_NONE={0,0,0,0,0,0,0};
const Abort_reason_codesMap ABORT_REASON_CODESMAP_ODD={1,0,1,0,1,0,1};

class Abort_reason_codesFactory{
public:
	static bool getEnumByValue(uint32_t value,enum Abort_reason_codesEnum& res);
	static bool getValueByEnum(enum Abort_reason_codesEnum value,uint32_t& res);

	static bool getEnumMapByValue(uint32_t value,Abort_reason_codesMap& res);
	static bool getValueByEnumMap(Abort_reason_codesMap value,uint32_t& res);
	static bool mapEquals(Abort_reason_codesMap val1,Abort_reason_codesMap val2);
};//enum
};//namespace
#endif

