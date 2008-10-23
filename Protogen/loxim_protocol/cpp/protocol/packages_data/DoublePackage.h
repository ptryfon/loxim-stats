#ifndef DOUBLEPACKAGE_H_
#define DOUBLEPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_DoublePackage 17

class DoublePackage : public Package
{
	private:
		double value;
	public:
		DoublePackage();
		~DoublePackage();
		DoublePackage(
			double a_value
			);

		virtual bool equals(Package* _package);

		double getValue(){return value;};
		void setValue(double a_value){value=a_value;};


		uint8_t getPackageType(){return ID_DoublePackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define DOUBLEPACKAGE_H_*/
