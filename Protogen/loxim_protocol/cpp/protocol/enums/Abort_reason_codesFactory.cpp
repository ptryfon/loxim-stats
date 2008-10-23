#include "Abort_reason_codesFactory.h"

using namespace protocol;


bool Abort_reason_codesFactory::getEnumByValue(uint32_t value,enum Abort_reason_codesEnum& res){
	if ((value==1)){res= arc_administration_reason;return true;};
	if ((value==2)){res= arc_transaction_victim;return true;};
	if ((value==3)){res= arc_operation_not_permited;return true;};
	if ((value==4)){res= arc_time_limit_exceeded;return true;};
	if ((value==5)){res= arc_out_of_memory;return true;};
	if ((value==6)){res= arc_type_check_error;return true;};
	if ((value==7)){res= arc_other_run_time_error;return true;};
	return false;
}

bool Abort_reason_codesFactory::getValueByEnum(enum Abort_reason_codesEnum value,uint32_t& res){
	switch (value){
		case arc_administration_reason: res= 1;return true;
		case arc_transaction_victim: res= 2;return true;
		case arc_operation_not_permited: res= 3;return true;
		case arc_time_limit_exceeded: res= 4;return true;
		case arc_out_of_memory: res= 5;return true;
		case arc_type_check_error: res= 6;return true;
		case arc_other_run_time_error: res= 7;return true;
		default: return false;
	};
}
bool Abort_reason_codesFactory::getEnumMapByValue(uint32_t value,Abort_reason_codesMap& res){
	res.arc_administration_reason=(((value&1)==1)?1:0);
	res.arc_transaction_victim=(((value&2)==2)?1:0);
	res.arc_operation_not_permited=(((value&3)==3)?1:0);
	res.arc_time_limit_exceeded=(((value&4)==4)?1:0);
	res.arc_out_of_memory=(((value&5)==5)?1:0);
	res.arc_type_check_error=(((value&6)==6)?1:0);
	res.arc_other_run_time_error=(((value&7)==7)?1:0);
	return true;
}

bool Abort_reason_codesFactory::getValueByEnumMap(Abort_reason_codesMap value,uint32_t& res){
	res=0;
	if (value.arc_administration_reason) res|=1;
	if (value.arc_transaction_victim) res|=2;
	if (value.arc_operation_not_permited) res|=3;
	if (value.arc_time_limit_exceeded) res|=4;
	if (value.arc_out_of_memory) res|=5;
	if (value.arc_type_check_error) res|=6;
	if (value.arc_other_run_time_error) res|=7;
	return true;
};
bool Abort_reason_codesFactory::mapEquals(Abort_reason_codesMap val1,Abort_reason_codesMap val2){
	return (val1.arc_administration_reason==val2.arc_administration_reason)&&(val1.arc_transaction_victim==val2.arc_transaction_victim)&&(val1.arc_operation_not_permited==val2.arc_operation_not_permited)&&(val1.arc_time_limit_exceeded==val2.arc_time_limit_exceeded)&&(val1.arc_out_of_memory==val2.arc_out_of_memory)&&(val1.arc_type_check_error==val2.arc_type_check_error)&&(val1.arc_other_run_time_error==val2.arc_other_run_time_error)&&(true);
};

