#ifndef BINDINGDATA_H_
#define BINDINGDATA_H_

#include "DataPart.h"

#define DATAPART_TYPE_BINDING	0x0082
namespace protocol{

class BindingData: public DataPart
{
	private:
		CharArray*  bindingName;
		uint64_t	valueId;
		uint64_t	embadedDataType;
		DataPart*	dataPart;
	
	public:
		BindingData();
		BindingData(
			CharArray*  a_bindingName,
			uint64_t	a_embadedDataType,
			DataPart*	a_dataPart);
		
		BindingData(
			uint64_t	valueId,
			uint64_t	a_embadedDataType,
			DataPart*	a_dataPart);
			
		virtual ~BindingData();
		
		//uint8_t getValue(){return value;};
		
		CharArray*  getBindingName(){return bindingName;};
		uint64_t	getValueId(){return valueId;};
		uint64_t	getEmbadedDataType(){return embadedDataType;};
		DataPart*	getDataPart(){return dataPart;};
		
		virtual uint64_t getDataType(){return DATAPART_TYPE_BINDING;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};

}
#endif /*BINDINGDATA_H_*/
