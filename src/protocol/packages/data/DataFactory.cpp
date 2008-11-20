#include <protocol/packages/data/DataFactory.h>

#include <protocol/packages/data/DataPart.h>

#include <protocol/packages/data/Uint8Data.h>
#include <protocol/packages/data/BagData.h>
#include <protocol/packages/data/BindingData.h>
#include <protocol/packages/data/BOBData.h>
#include <protocol/packages/data/BoolData.h>
#include <protocol/packages/data/CollectionData.h>
#include <protocol/packages/data/DataFactory.h>
#include <protocol/packages/data/DataPart.h>
#include <protocol/packages/data/DateData.h>
#include <protocol/packages/data/DateTimeData.h>
#include <protocol/packages/data/DateTimeTZData.h>
#include <protocol/packages/data/DoubleData.h>
#include <protocol/packages/data/ExtReferenceData.h>
#include <protocol/packages/data/LinkData.h>
#include <protocol/packages/data/ReferenceData.h>
#include <protocol/packages/data/SequenceData.h>
#include <protocol/packages/data/Sint16Data.h>
#include <protocol/packages/data/Sint32Data.h>
#include <protocol/packages/data/Sint64Data.h>
#include <protocol/packages/data/Sint8Data.h>
#include <protocol/packages/data/StructData.h>
#include <protocol/packages/data/TimeData.h>
#include <protocol/packages/data/TimeTZData.h>
#include <protocol/packages/data/Uint16Data.h>
#include <protocol/packages/data/Uint32Data.h>
#include <protocol/packages/data/Uint64Data.h>
#include <protocol/packages/data/Uint8Data.h>
#include <protocol/packages/data/VarcharData.h>
#include <protocol/packages/data/VOIDData.h>


using namespace protocol;


 DataPart* DataFactory::createDataPart(uint16_t type)
 {	
 	switch(type)
 	{
		case DATAPART_TYPE_BAG     	: return new BagData();
		case DATAPART_TYPE_BINDING 	: return new BindingData();
		case DATAPART_TYPE_BOB     	: return new BOBData();
		case DATAPART_TYPE_BOOL 		: return new BoolData();
		case DATAPART_TYPE_DATE 		: return new DateData();
		case DATAPART_TYPE_DATETIME 	: return new DateTimeData();
		case DATAPART_TYPE_DATETIMETZ 	: return new DateTimeTZData();
		case DATAPART_TYPE_DOUBLE 		: return new DoubleData();
		case DATAPART_TYPE_EXTREFERENCE : return new ExtReferenceData();
		case DATAPART_TYPE_LINK 		: return new LinkData();
		case DATAPART_TYPE_REFERENCE 	: return new ReferenceData();
		case DATAPART_TYPE_SEQUENCE 	: return new SequenceData();
		case DATAPART_TYPE_SINT16 		: return new Sint16Data();
		case DATAPART_TYPE_SINT32 		: return new Sint32Data();
		case DATAPART_TYPE_SINT64 		: return new Sint64Data();
		case DATAPART_TYPE_SINT8 		: return new Sint8Data();
		case DATAPART_TYPE_STRUCT 		: return new StructData();
		case DATAPART_TYPE_TIME 		: return new TimeData();
		case DATAPART_TYPE_TIMETZ 		: return new TimeTZData();
		case DATAPART_TYPE_UINT16 		: return new Uint16Data();
		case DATAPART_TYPE_UINT32 		: return new Uint32Data();
		case DATAPART_TYPE_UINT64		: return new Uint64Data();
		case DATAPART_TYPE_UINT8 		: return new Uint8Data();
		case DATAPART_TYPE_VARCHAR 	: return new VarcharData();
		case DATAPART_TYPE_VOID		: return new VOIDData();
 		
 		default:
 			return NULL; 			
 	};
 }
