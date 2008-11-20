#ifndef WSHELLOPACKAGE_H_
#define WSHELLOPACKAGE_H_

#include <stdint.h> 
#include <protocol/ptools/Package.h>

#define ID_WSHelloPackage 11

namespace protocol{

class WSHelloPackage:public Package
{
	private:
		uint8_t p_minor;
		uint8_t p_major;
		uint8_t s_minor;
		uint8_t s_major;
		uint32_t maxPackageSize;
		uint64_t featuresMap;
		uint64_t authMethodsMap;
		char salt[20];		
	
	public: 
		WSHelloPackage(
			uint8_t a_p_major,\
			uint8_t a_p_minor,\
			uint8_t a_s_major,\
			uint8_t a_s_minor,\
			uint32_t a_maxPackageSize,\
			uint64_t a_featuresMap,\
			uint64_t a_authMethodsMap,\
			char a_salt[20]);
			
		WSHelloPackage(\
			uint32_t a_maxPackageSize,\
			uint64_t a_featuresMap,\
			uint64_t a_authMethodsMap);
			
		WSHelloPackage();
			
		uint8_t getPackageType(){return ID_WSHelloPackage;};
		
		uint8_t getP_minor(){return p_minor;};
		uint8_t getP_major(){return p_major;};
		uint8_t getS_minor(){return s_minor;};
		uint8_t getS_major(){return s_major;};
		uint32_t getMaxPackageSize(){return maxPackageSize;};
		uint64_t getFeaturesMap(){return featuresMap;};
		uint64_t getAuthMethodsMap(){return authMethodsMap;};
		char* getSalt(){return salt;};		  
		
		virtual bool equals(Package* package);
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
		
		
	
};
}
#endif /*WSHELLOPACKAGE_H_*/
