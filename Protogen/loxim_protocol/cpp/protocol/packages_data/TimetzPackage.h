#ifndef TIMETZPACKAGE_H_
#define TIMETZPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"
#include "../packages_data/TimePackage.h"

namespace protocol{

#define ID_TimetzPackage 13

class TimetzPackage : public Package
{
	private:
		TimePackage* time;
		int8_t tz;
	public:
		TimetzPackage();
		~TimetzPackage();
		TimetzPackage(
			TimePackage* a_time,\
			int8_t a_tz
			);

		virtual bool equals(Package* _package);

		TimePackage* getTime(){return time;};
		void setTime(TimePackage* a_time){time=a_time;};

		int8_t getTz(){return tz;};
		void setTz(int8_t a_tz){tz=a_tz;};


		uint8_t getPackageType(){return ID_TimetzPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define TIMETZPACKAGE_H_*/
