#include <string.h>
#include <stdlib.h>
#include "V_sc_sendvaluesPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

V_sc_sendvaluesPackage::V_sc_sendvaluesPackage(\
		int64_t a_rootValueId,\
		int64_t a_oBundlesCount,\
		int64_t a_oBidCount,\
		int64_t a_pVidCount
		,bool a_bnull_oBundlesCount
		,bool a_bnull_oBidCount
		,bool a_bnull_pVidCount
		)
{
	rootValueId=a_rootValueId;
	oBundlesCount=a_oBundlesCount;
	bnull_oBundlesCount=a_bnull_oBundlesCount;
	oBidCount=a_oBidCount;
	bnull_oBidCount=a_bnull_oBidCount;
	pVidCount=a_pVidCount;
	bnull_pVidCount=a_bnull_pVidCount;
}

V_sc_sendvaluesPackage::V_sc_sendvaluesPackage()
{
	rootValueId=0;
	oBundlesCount=0;
	bnull_oBundlesCount=false;
	oBidCount=0;
	bnull_oBidCount=false;
	pVidCount=0;
	bnull_pVidCount=false;
}

V_sc_sendvaluesPackage::~V_sc_sendvaluesPackage()
{
}

void V_sc_sendvaluesPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	writer->writeVaruint(rootValueId);
	if (bnull_oBundlesCount) writer->writeVaruintNull(); else writer->writeVaruint(oBundlesCount);
	if (bnull_oBidCount) writer->writeVaruintNull(); else writer->writeVaruint(oBidCount);
	if (bnull_pVidCount) writer->writeVaruintNull(); else writer->writeVaruint(pVidCount);
}

bool V_sc_sendvaluesPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	{bool b; rootValueId=reader->readVaruint(b);}
	oBundlesCount=reader->readVaruint(bnull_oBundlesCount);
	oBidCount=reader->readVaruint(bnull_oBidCount);
	pVidCount=reader->readVaruint(bnull_pVidCount);
	return true;
}

bool V_sc_sendvaluesPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	V_sc_sendvaluesPackage* p=(V_sc_sendvaluesPackage*)_package;
	if (!(getRootValueId()==p->getRootValueId())) return false;
	if (getBnull_oBundlesCount()!=p->getBnull_oBundlesCount()) return false;
	if ((!bnull_oBundlesCount)&&(!(getOBundlesCount()==p->getOBundlesCount()))) return false;
	if (getBnull_oBidCount()!=p->getBnull_oBidCount()) return false;
	if ((!bnull_oBidCount)&&(!(getOBidCount()==p->getOBidCount()))) return false;
	if (getBnull_pVidCount()!=p->getBnull_pVidCount()) return false;
	if ((!bnull_pVidCount)&&(!(getPVidCount()==p->getPVidCount()))) return false;
	return true;
}
