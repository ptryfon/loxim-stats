#ifndef COLLECTIONDATA_H_
#define COLLECTIONDATA_H_

#include <protocol/packages/data/DataPart.h>

namespace protocol{

class CollectionData: public DataPart
{
	private:
		uint64_t   count;
		uint64_t   globalType;
		bool	   bnull_globalType;
		DataPart** dataParts;
	
	public:
		CollectionData();
		/*
		 * Konstruktor sam okre¶la, pozosta³e pola.
		 * Przekazana tablica obiektów jest zwalniana w destruktorze tej klasy
		 */ 
		CollectionData(uint64_t a_count,DataPart** a_dataParts);
		virtual ~CollectionData();
		
		uint64_t getCount(){return count;};
		uint64_t   getGlobalType(){return globalType;};
		bool	   getBnull_globalType(){return bnull_globalType;};
		DataPart** getDataParts(){return dataParts;};
		
		virtual bool equals(DataPart* package);
		
		virtual void serializeW(PackageBufferWriter *writter);
				
		virtual bool deserializeR(PackageBufferReader *reader);		
};
}
#endif /*COLLECTIONDATA_H_*/
