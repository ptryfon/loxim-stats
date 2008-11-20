#ifndef DATAPART_H_
#define DATAPART_H_

#include <stdint.h>
#include <stdlib.h>
#include "../../ptools/PackageBufferWriter.h"
#include "../../ptools/PackageBufferReader.h"

namespace protocol{


class DataPart
{
	public:
		DataPart(){};
		virtual ~DataPart(){};
		
		virtual uint64_t getDataType()=0;
		
		virtual bool equals(DataPart* package)=0;
		
		virtual void serializeW(PackageBufferWriter *writter)=0;
				
		/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
		virtual bool deserializeR(PackageBufferReader *reader)=0;
		
		static bool equal(DataPart* p1,DataPart* p2);
};
}
#endif /*DATAPART_H_*/
