#ifndef A_SC_SETOPTPACKAGE_H_
#define A_SC_SETOPTPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_A_sc_setoptPackage 4

class A_sc_setoptPackage : public Package
{
	private:
		CharArray* key;
		CharArray* value;
	public:
		A_sc_setoptPackage();
		~A_sc_setoptPackage();
		A_sc_setoptPackage(
			CharArray* a_key,\
			CharArray* a_value
			);

		virtual bool equals(Package* _package);

		CharArray* getKey(){return key;};
		void setKey(CharArray* a_key){key=a_key;};

		CharArray* getValue(){return value;};
		void setValue(CharArray* a_value){value=a_value;};


		uint8_t getPackageType(){return ID_A_sc_setoptPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define A_SC_SETOPTPACKAGE_H_*/
