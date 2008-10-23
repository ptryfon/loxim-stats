#ifndef V_SC_SENDVALUEPACKAGE_H_
#define V_SC_SENDVALUEPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"
#include "../packages_data/DataPackagesFactory.h"

namespace protocol{

#define ID_V_sc_sendvaluePackage 33

class V_sc_sendvaluePackage : public Package
{
	private:
		int64_t value_id;
		struct Send_value_flagsMap flags;
		int64_t value_type;
		Package* data;
	public:
		V_sc_sendvaluePackage();
		~V_sc_sendvaluePackage();
		V_sc_sendvaluePackage(
			int64_t a_value_id,\
			struct Send_value_flagsMap a_flags,\
			int64_t a_value_type,\
			Package* a_data
			);

		virtual bool equals(Package* _package);

		int64_t getValue_id(){return value_id;};
		void setValue_id(int64_t a_value_id){value_id=a_value_id;};

		struct Send_value_flagsMap getFlags(){return flags;};
		void setFlags(struct Send_value_flagsMap a_flags){flags=a_flags;};

		int64_t getValue_type(){return value_type;};
		void setValue_type(int64_t a_value_type){value_type=a_value_type;};

		Package* getData(){return data;};
		void setData(Package* a_data){data=a_data;};


		uint8_t getPackageType(){return ID_V_sc_sendvaluePackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define V_SC_SENDVALUEPACKAGE_H_*/
