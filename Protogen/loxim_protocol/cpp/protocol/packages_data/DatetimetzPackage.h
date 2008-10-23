#ifndef DATETIMETZPACKAGE_H_
#define DATETIMETZPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"
#include "../packages_data/DatePackage.h"
#include "../packages_data/TimePackage.h"

namespace protocol{

#define ID_DatetimetzPackage 14

class DatetimetzPackage : public Package
{
	private:
		DatePackage* date;
		TimePackage* time;
		int8_t tz;
	public:
		DatetimetzPackage();
		~DatetimetzPackage();
		DatetimetzPackage(
			DatePackage* a_date,\
			TimePackage* a_time,\
			int8_t a_tz
			);

		virtual bool equals(Package* _package);

		DatePackage* getDate(){return date;};
		void setDate(DatePackage* a_date){date=a_date;};

		TimePackage* getTime(){return time;};
		void setTime(TimePackage* a_time){time=a_time;};

		int8_t getTz(){return tz;};
		void setTz(int8_t a_tz){tz=a_tz;};


		uint8_t getPackageType(){return ID_DatetimetzPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define DATETIMETZPACKAGE_H_*/
