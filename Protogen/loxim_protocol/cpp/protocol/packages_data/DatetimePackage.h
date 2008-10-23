#ifndef DATETIMEPACKAGE_H_
#define DATETIMEPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"
#include "../packages_data/DatePackage.h"
#include "../packages_data/TimePackage.h"

namespace protocol{

#define ID_DatetimePackage 12

class DatetimePackage : public Package
{
	private:
		DatePackage* date;
		TimePackage* time;
	public:
		DatetimePackage();
		~DatetimePackage();
		DatetimePackage(
			DatePackage* a_date,\
			TimePackage* a_time
			);

		virtual bool equals(Package* _package);

		DatePackage* getDate(){return date;};
		void setDate(DatePackage* a_date){date=a_date;};

		TimePackage* getTime(){return time;};
		void setTime(TimePackage* a_time){time=a_time;};


		uint8_t getPackageType(){return ID_DatetimePackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define DATETIMEPACKAGE_H_*/
