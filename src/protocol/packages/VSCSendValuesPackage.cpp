#include <string.h>
#include <protocol/packages/VSCSendValuesPackage.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>

#include <stdlib.h>

using namespace protocol;

VSCSendValuesPackage::VSCSendValuesPackage()
{
	rootValueId=0;
	oBundlesCount=0;
	oBidCount=0;
	pVidCount=0;
	bnull_oBundlesCount=true;
	bnull_oBidCount=true;
	bnull_pVidCount=true;
}

VSCSendValuesPackage::VSCSendValuesPackage(
			uint64_t a_rootValueId,
			uint64_t a_oBundlesCount, //Orientacyjna liczba paczek, które zostan± przes³ane (NULL - nie okre¶lone)
			uint64_t a_oBidCount,		//Orientacyjna liczba obiektów, które zostan± przes³ane (NULL - nie okre¶lone)
			uint64_t a_pVidCount,		//Dok³adna liczba obiektów, które zostan± przs³ane (NULL - nie okre¶lone)
			bool a_bnull_oBundlesCount/*=false*/, 
			bool a_bnull_oBidCount/*=false*/,
			bool a_bnull_pVidCount/*=false*/)
{
	rootValueId=a_rootValueId;
	oBundlesCount=a_oBundlesCount;
	oBidCount=a_oBidCount;
	pVidCount=a_pVidCount;
	bnull_oBundlesCount=a_bnull_oBundlesCount;
	bnull_oBidCount=a_bnull_oBidCount;
	bnull_pVidCount=a_bnull_pVidCount;
}
			
void VSCSendValuesPackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeVaruint(rootValueId);
	
	if (bnull_oBundlesCount)
		  writer->writeVaruintNull();
	else writer->writeVaruint(oBundlesCount);
	
	if (bnull_oBidCount)
		  writer->writeVaruintNull();
	else writer->writeVaruint(oBidCount);
	
	if (bnull_pVidCount)
		  writer->writeVaruintNull();
	else writer->writeVaruint(pVidCount);	
}

bool  VSCSendValuesPackage::deserializeR(PackageBufferReader *reader)
{
	bool null;
	rootValueId		=reader->readVaruint(null);
	if (null)
		return false; //pole rootValueId nie mo¿e dostaæ warto¶ci NULL
		 
	oBundlesCount	=reader->readVaruint(bnull_oBundlesCount);
	oBidCount		=reader->readVaruint(bnull_oBidCount);
	pVidCount		=reader->readVaruint(bnull_pVidCount);
	return true;
}

bool VSCSendValuesPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 		
  	VSCSendValuesPackage *pack=(VSCSendValuesPackage*)package;
  	if (pack->getRootValueId()!=getRootValueId())
 		return false;
 		
 	if ( (pack->getBnull_oBundlesCount() != getBnull_oBundlesCount())
 		||(pack->getBnull_oBidCount() != getBnull_oBidCount())
 		||(pack->getBnull_pVidCount() != getBnull_pVidCount()))
 		return false;
 	
 	if ((!getBnull_oBundlesCount())&&(pack->getOBundlesCount() != getOBundlesCount()))
 		return false;
 		
 	if ((!getBnull_oBidCount())&&(pack->getOBidCount() != getOBidCount()))
 		return false;	
 	
 	if ((!getBnull_pVidCount())&&(pack->getPVidCount() != getPVidCount()))
 		return false;	
 	 	
   	return true;
 }


