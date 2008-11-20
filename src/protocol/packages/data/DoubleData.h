#ifndef DOUBLEDATA_H_
#define DOUBLEDATA_H_

#include "DataPart.h"

#define DATAPART_TYPE_DOUBLE	0x0011
namespace protocol{


class DoubleData: public DataPart
{
	private:
		double value;
	
	public:
		DoubleData();
		DoubleData(double a_value);
		virtual ~DoubleData();
		
		double getValue(){return value;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_DOUBLE;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*DOUBLEDATA_H_*/
