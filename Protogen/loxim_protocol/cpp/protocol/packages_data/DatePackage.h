#ifndef DATEPACKAGE_H_
#define DATEPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_DatePackage 10

class DatePackage : public Package
{
	private:
		int16_t year;
		uint8_t month;
		uint8_t day;
	public:
		DatePackage();
		~DatePackage();
		DatePackage(
			int16_t a_year,\
			uint8_t a_month,\
			uint8_t a_day
			);

		virtual bool equals(Package* _package);

		int16_t getYear(){return year;};
		void setYear(int16_t a_year){year=a_year;};

		uint8_t getMonth(){return month;};
		void setMonth(uint8_t a_month){month=a_month;};

		uint8_t getDay(){return day;};
		void setDay(uint8_t a_day){day=a_day;};


		uint8_t getPackageType(){return ID_DatePackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define DATEPACKAGE_H_*/
