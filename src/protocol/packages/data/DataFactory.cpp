#include "DataFactory.h"

#include "DataPart.h"

#include "Uint8Data.h"
#include "BagData.h"
#include "BindingData.h"
#include "BOBData.h"
#include "BoolData.h"
#include "CollectionData.h"
#include "DataFactory.h"
#include "DataPart.h"
#include "DateData.h"
#include "DateTimeData.h"
#include "DateTimeTZData.h"
#include "DoubleData.h"
#include "ExtReferenceData.h"
#include "LinkData.h"
#include "ReferenceData.h"
#include "SequenceData.h"
#include "Sint16Data.h"
#include "Sint32Data.h"
#include "Sint64Data.h"
#include "Sint8Data.h"
#include "StructData.h"
#include "TimeData.h"
#include "TimeTZData.h"
#include "Uint16Data.h"
#include "Uint32Data.h"
#include "Uint64Data.h"
#include "Uint8Data.h"
#include "VarcharData.h"
#include "VOIDData.h"


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
