#include "Statement_flagsFactory.h"

using namespace protocol;


bool Statement_flagsFactory::getEnumByValue(uint64_t value,enum Statement_flagsEnum& res){
	if ((value==0x0001)){res= sf_execute;return true;};
	if ((value==0x0002)){res= sf_readonly;return true;};
	if ((value==0x0010)){res= sf_prefer_dfs;return true;};
	if ((value==0x0020)){res= sf_prefer_bsf;return true;};
	return false;
}

bool Statement_flagsFactory::getValueByEnum(enum Statement_flagsEnum value,uint64_t& res){
	switch (value){
		case sf_execute: res= 0x0001;return true;
		case sf_readonly: res= 0x0002;return true;
		case sf_prefer_dfs: res= 0x0010;return true;
		case sf_prefer_bsf: res= 0x0020;return true;
		default: return false;
	};
}
bool Statement_flagsFactory::getEnumMapByValue(uint64_t value,Statement_flagsMap& res){
	res.sf_execute=(((value&0x0001)==0x0001)?1:0);
	res.sf_readonly=(((value&0x0002)==0x0002)?1:0);
	res.sf_prefer_dfs=(((value&0x0010)==0x0010)?1:0);
	res.sf_prefer_bsf=(((value&0x0020)==0x0020)?1:0);
	return true;
}

bool Statement_flagsFactory::getValueByEnumMap(Statement_flagsMap value,uint64_t& res){
	res=0;
	if (value.sf_execute) res|=0x0001;
	if (value.sf_readonly) res|=0x0002;
	if (value.sf_prefer_dfs) res|=0x0010;
	if (value.sf_prefer_bsf) res|=0x0020;
	return true;
};
bool Statement_flagsFactory::mapEquals(Statement_flagsMap val1,Statement_flagsMap val2){
	return (val1.sf_execute==val2.sf_execute)&&(val1.sf_readonly==val2.sf_readonly)&&(val1.sf_prefer_dfs==val2.sf_prefer_dfs)&&(val1.sf_prefer_bsf==val2.sf_prefer_bsf)&&(true);
};

