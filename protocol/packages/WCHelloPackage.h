#ifndef WCHELLOPACKAGE_H_
#define WCHELLOPACKAGE_H_

#include <stdint.h> 
#include "../ptools/Package.h"

#define ID_WCHelloPackage 10

namespace protocol{

class WCHelloPackage: public Package
{
	private:
		int64_t pid;
		CharArray* programName;
		CharArray* programVersion;
		CharArray* hostname;
		CharArray* language; 
		uint64_t collation;
		int8_t timezone;
	
	public:
		//Kopiuje napisy 
		WCHelloPackage(
			int64_t a_pid,
			char* a_programName,\
			char* a_programVersion,\
			char* a_hostname,\
			char* a_language,\
			uint64_t a_collation,\
			int8_t a_timezone );
		
		/*Nie kopiuje napisów*/	
		WCHelloPackage(
			int64_t a_pid,
			CharArray* a_programName,\
			CharArray* a_programVersion,\
			CharArray* a_hostname,\
			CharArray* a_language,\
			uint64_t a_collation,\
			int8_t a_timezone );
		
		WCHelloPackage();
		~WCHelloPackage();
			
		uint8_t getPackageType(){return ID_WCHelloPackage;};
		
		int64_t 	getPid(){return pid;};
		CharArray* 	getProgramName(){return programName;};
		CharArray* 	getProgramVersion(){return programVersion;};
		CharArray* 	getHostname(){return hostname;};
		CharArray* 	getLanguage(){return language;}; 
		uint64_t 	getCollation(){return collation;};
		int8_t 		getTimezone(){return timezone;};
		
		virtual bool equals(Package* package);  
		
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
	
};
}
#endif /*WCHELLOPACKAGE_H_*/
