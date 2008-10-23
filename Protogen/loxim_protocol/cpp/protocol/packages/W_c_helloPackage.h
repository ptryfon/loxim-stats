#ifndef W_C_HELLOPACKAGE_H_
#define W_C_HELLOPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_W_c_helloPackage 10

class W_c_helloPackage : public Package
{
	private:
		int64_t pid;
		CharArray* client_name;
		CharArray* client_version;
		CharArray* hostname;
		CharArray* language;
		enum CollationsEnum collation;
		int8_t timezone;
	public:
		W_c_helloPackage();
		~W_c_helloPackage();
		W_c_helloPackage(
			int64_t a_pid,\
			CharArray* a_client_name,\
			CharArray* a_client_version,\
			CharArray* a_hostname,\
			CharArray* a_language,\
			enum CollationsEnum a_collation,\
			int8_t a_timezone
			);

		virtual bool equals(Package* _package);

		int64_t getPid(){return pid;};
		void setPid(int64_t a_pid){pid=a_pid;};

		CharArray* getClient_name(){return client_name;};
		void setClient_name(CharArray* a_client_name){client_name=a_client_name;};

		CharArray* getClient_version(){return client_version;};
		void setClient_version(CharArray* a_client_version){client_version=a_client_version;};

		CharArray* getHostname(){return hostname;};
		void setHostname(CharArray* a_hostname){hostname=a_hostname;};

		CharArray* getLanguage(){return language;};
		void setLanguage(CharArray* a_language){language=a_language;};

		enum CollationsEnum getCollation(){return collation;};
		void setCollation(enum CollationsEnum a_collation){collation=a_collation;};

		int8_t getTimezone(){return timezone;};
		void setTimezone(int8_t a_timezone){timezone=a_timezone;};


		uint8_t getPackageType(){return ID_W_c_helloPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define W_C_HELLOPACKAGE_H_*/
