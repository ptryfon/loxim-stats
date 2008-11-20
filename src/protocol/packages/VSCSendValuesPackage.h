#ifndef VSCSENDVALUESPACKAGE_H_
#define VSCSENDVALUESPACKAGE_H_

#include <stdint.h> 
#include "../ptools/Package.h"

#define ID_VSCSendValuesPackage 32

namespace protocol{

class VSCSendValuesPackage:public Package
{
	private:		
		uint64_t rootValueId;
		uint64_t oBundlesCount; //Orientacyjna liczba paczek, które zostan± przes³ane (NULL - nie okre¶lone)
		uint64_t oBidCount;		//Orientacyjna liczba obiektów, które zostan± przes³ane (NULL - nie okre¶lone)
		uint64_t pVidCount;		//Dok³adna liczba obiektów, które zostan± przs³ane (NULL - nie okre¶lone)
		bool bnull_oBundlesCount; 
		bool bnull_oBidCount;	
		bool bnull_pVidCount;		
	
	public: 
		VSCSendValuesPackage(
			uint64_t rootValueId,
			uint64_t oBundlesCount, //Orientacyjna liczba paczek, które zostan± przes³ane (NULL - nie okre¶lone)
			uint64_t oBidCount,		//Orientacyjna liczba obiektów, które zostan± przes³ane (NULL - nie okre¶lone)
			uint64_t pVidCount,		//Dok³adna liczba obiektów, które zostan± przs³ane (NULL - nie okre¶lone)
			bool bnull_oBundlesCount=false, 
			bool bnull_oBidCount=false,
			bool bnull_pVidCount=false);
		
		VSCSendValuesPackage();
			
		uint8_t getPackageType(){return ID_VSCSendValuesPackage;};
		
		virtual bool equals(Package* package);
		
		uint64_t getRootValueId(){return rootValueId;};		
		uint64_t getOBundlesCount(){return oBundlesCount;}; 
		uint64_t getOBidCount(){return oBidCount;};
		uint64_t getPVidCount(){return pVidCount;};
		bool getBnull_oBundlesCount(){return bnull_oBundlesCount;};
		bool getBnull_oBidCount(){return bnull_oBidCount;};
		bool getBnull_pVidCount(){return bnull_pVidCount;};
		
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*VSCSENDVALUESPACKAGE_H_*/
