#ifndef COLLECTIONPACKAGE_H_
#define COLLECTIONPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol {

class CollectionPackage : public Package {
private:
	uint64_t count;
	uint64_t globalType;
	bool bnull_globalType;
	Package* *dataParts;

public:
	CollectionPackage();
	CollectionPackage(uint64_t a_count,Package** a_dataParts);
	
	~CollectionPackage();

	virtual bool equals(Package* _package);

	uint8_t getPackageType()=0;
	
	uint64_t getCount(){return count;};
	uint64_t getGlobalType(){return globalType;};
	bool getBnull_globalType(){return bnull_globalType;};
	Package **getDataParts(){return dataParts;};
	void setCount(uint64_t c){count=c;};
	void setGlobalType(uint64_t c){globalType=c;};
	void setBnull_globalType(bool b){bnull_globalType=b;};
	void setDataParts(Package** a_dataParts){dataParts=a_dataParts;};
protected:
	void serializeW(PackageBufferWriter *writter);
	bool deserializeR(PackageBufferReader *reader);
};
//class

}//namespace
#endif /*define COLLECTIONPACKAGE_H_*/
