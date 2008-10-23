#include <string.h>
#include <stdlib.h>
#include "A_sc_errorPackage.h"
#include "ptools/PackageBufferWriter.h"
#include "ptools/PackageBufferReader.h"

using namespace protocol;

A_sc_errorPackage::A_sc_errorPackage(\
		enum ErrorsEnum a_error_code,\
		int64_t a_error_object_id,\
		CharArray* a_description,\
		uint32_t a_line,\
		uint32_t a_col
		,bool a_bnull_error_object_id
		)
{
	error_code=a_error_code;
	error_object_id=a_error_object_id;
	bnull_error_object_id=a_bnull_error_object_id;
	description=a_description;
	line=a_line;
	col=a_col;
}

A_sc_errorPackage::A_sc_errorPackage()
{
	error_object_id=0;
	bnull_error_object_id=false;
	description=NULL;
	line=0;
	col=0;
}

A_sc_errorPackage::~A_sc_errorPackage()
{
	if (description) delete description;
}

void A_sc_errorPackage::serializeW(PackageBufferWriter *writer)
{
	Package::serializeW(writer);
	uint32_t tmp_error_code;
	ErrorsFactory::getValueByEnum(error_code, tmp_error_code);
	writer->writeUint32(tmp_error_code);
	if (bnull_error_object_id) writer->writeVaruintNull(); else writer->writeVaruint(error_object_id);
	writer->writeSstring(description);
	writer->writeUint32(line);
	writer->writeUint32(col);
}

bool A_sc_errorPackage::deserializeR(PackageBufferReader *reader)
{
	if (!Package::deserializeR(reader)) return false;
	ErrorsFactory::getEnumByValue(reader->readUint32(),error_code);
	error_object_id=reader->readVaruint(bnull_error_object_id);
	description=reader->readSstring();
	line=reader->readUint32();
	col=reader->readUint32();
	return true;
}

bool A_sc_errorPackage::equals(Package *_package)
{
	if (!_package) return false;
	if (_package->getPackageType()!=this->getPackageType()) return false;
	if (!Package::equals(_package)) return false;
	A_sc_errorPackage* p=(A_sc_errorPackage*)_package;
	if (!(getError_code()==p->getError_code())) return false;
	if (getBnull_error_object_id()!=p->getBnull_error_object_id()) return false;
	if ((!bnull_error_object_id)&&(!(getError_object_id()==p->getError_object_id()))) return false;
	if (!(CharArray::equal(getDescription(),p->getDescription()))) return false;
	if (!(getLine()==p->getLine())) return false;
	if (!(getCol()==p->getCol())) return false;
	return true;
}
