#ifndef Q_S_EXECUTINGPACKAGE_H_
#define Q_S_EXECUTINGPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_Q_s_executingPackage 67

class Q_s_executingPackage : public Package
{
	private:
	public:
		Q_s_executingPackage();
		~Q_s_executingPackage();

		virtual bool equals(Package* _package);


		uint8_t getPackageType(){return ID_Q_s_executingPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define Q_S_EXECUTINGPACKAGE_H_*/
