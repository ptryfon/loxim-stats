#ifndef W_C_MODEPACKAGE_H_
#define W_C_MODEPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_W_c_modePackage 12

class W_c_modePackage : public Package
{
	private:
		enum ModesEnum new_mode;
	public:
		W_c_modePackage();
		~W_c_modePackage();
		W_c_modePackage(
			enum ModesEnum a_new_mode
			);

		virtual bool equals(Package* _package);

		enum ModesEnum getNew_mode(){return new_mode;};
		void setNew_mode(enum ModesEnum a_new_mode){new_mode=a_new_mode;};


		uint8_t getPackageType(){return ID_W_c_modePackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define W_C_MODEPACKAGE_H_*/
