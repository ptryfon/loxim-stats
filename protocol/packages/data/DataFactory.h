#ifndef DATAFACTORY_H_
#define DATAFACTORY_H_

#include "DataPart.h"
namespace protocol{


class DataFactory
{
	public: 
		static DataPart* createDataPart(uint16_t type);	
};
}
#endif /*DATAFACTORY_H_*/