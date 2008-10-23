#ifndef TIMEPACKAGE_H_
#define TIMEPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_TimePackage 11

class TimePackage : public Package
{
	private:
		uint8_t hour;
		uint8_t minuts;
		uint8_t sec;
		int16_t milis;
	public:
		TimePackage();
		~TimePackage();
		TimePackage(
			uint8_t a_hour,\
			uint8_t a_minuts,\
			uint8_t a_sec,\
			int16_t a_milis
			);

		virtual bool equals(Package* _package);

		uint8_t getHour(){return hour;};
		void setHour(uint8_t a_hour){hour=a_hour;};

		uint8_t getMinuts(){return minuts;};
		void setMinuts(uint8_t a_minuts){minuts=a_minuts;};

		uint8_t getSec(){return sec;};
		void setSec(uint8_t a_sec){sec=a_sec;};

		int16_t getMilis(){return milis;};
		void setMilis(int16_t a_milis){milis=a_milis;};


		uint8_t getPackageType(){return ID_TimePackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define TIMEPACKAGE_H_*/
