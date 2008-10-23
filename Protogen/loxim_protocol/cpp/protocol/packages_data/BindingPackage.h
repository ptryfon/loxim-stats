#ifndef BINDINGPACKAGE_H_
#define BINDINGPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"
#include "../packages_data/DataPackagesFactory.h"

namespace protocol{

#define ID_BindingPackage 130

class BindingPackage : public Package
{
	private:
		CharArray* bindingName;
		int64_t type;
		Package* value;
	public:
		BindingPackage();
		~BindingPackage();
		BindingPackage(
			CharArray* a_bindingName,\
			int64_t a_type,\
			Package* a_value
			);

		virtual bool equals(Package* _package);

		CharArray* getBindingName(){return bindingName;};
		void setBindingName(CharArray* a_bindingName){bindingName=a_bindingName;};

		int64_t getType(){return type;};
		void setType(int64_t a_type){type=a_type;};

		Package* getValue(){return value;};
		void setValue(Package* a_value){value=a_value;};


		uint8_t getPackageType(){return ID_BindingPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define BINDINGPACKAGE_H_*/
