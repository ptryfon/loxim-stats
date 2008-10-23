#include "ErrorsFactory.h"

using namespace protocol;


bool ErrorsFactory::getEnumByValue(uint32_t value,enum ErrorsEnum& res){
	if ((value==0x0000)){res= error_internal;return true;};
	if ((value==0x0001)){res= error_params_incomplete;return true;};
	if ((value==0x0002)){res= error_no_such_value_id;return true;};
	if ((value==0x0003)){res= error_operation_not_permited;return true;};
	return false;
}

bool ErrorsFactory::getValueByEnum(enum ErrorsEnum value,uint32_t& res){
	switch (value){
		case error_internal: res= 0x0000;return true;
		case error_params_incomplete: res= 0x0001;return true;
		case error_no_such_value_id: res= 0x0002;return true;
		case error_operation_not_permited: res= 0x0003;return true;
		default: return false;
	};
}
bool ErrorsFactory::getEnumMapByValue(uint32_t value,ErrorsMap& res){
	res.error_internal=(((value&0x0000)==0x0000)?1:0);
	res.error_params_incomplete=(((value&0x0001)==0x0001)?1:0);
	res.error_no_such_value_id=(((value&0x0002)==0x0002)?1:0);
	res.error_operation_not_permited=(((value&0x0003)==0x0003)?1:0);
	return true;
}

bool ErrorsFactory::getValueByEnumMap(ErrorsMap value,uint32_t& res){
	res=0;
	if (value.error_internal) res|=0x0000;
	if (value.error_params_incomplete) res|=0x0001;
	if (value.error_no_such_value_id) res|=0x0002;
	if (value.error_operation_not_permited) res|=0x0003;
	return true;
};
bool ErrorsFactory::mapEquals(ErrorsMap val1,ErrorsMap val2){
	return (val1.error_internal==val2.error_internal)&&(val1.error_params_incomplete==val2.error_params_incomplete)&&(val1.error_no_such_value_id==val2.error_no_such_value_id)&&(val1.error_operation_not_permited==val2.error_operation_not_permited)&&(true);
};

