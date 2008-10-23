#ifndef W_S_HELLOPACKAGE_H_
#define W_S_HELLOPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_W_s_helloPackage 11

class W_s_helloPackage : public Package
{
	private:
		uint8_t protocol_major;
		uint8_t protocol_minor;
		uint8_t system_major;
		uint8_t system_minor;
		uint32_t max_package_size;
		struct FeaturesMap features;
		struct Auth_methodsMap auth_methods;
		CharArray* salt;
	public:
		W_s_helloPackage();
		~W_s_helloPackage();
		W_s_helloPackage(
			uint8_t a_protocol_major,\
			uint8_t a_protocol_minor,\
			uint8_t a_system_major,\
			uint8_t a_system_minor,\
			uint32_t a_max_package_size,\
			struct FeaturesMap a_features,\
			struct Auth_methodsMap a_auth_methods,\
			CharArray* a_salt
			);

		virtual bool equals(Package* _package);

		uint8_t getProtocol_major(){return protocol_major;};
		void setProtocol_major(uint8_t a_protocol_major){protocol_major=a_protocol_major;};

		uint8_t getProtocol_minor(){return protocol_minor;};
		void setProtocol_minor(uint8_t a_protocol_minor){protocol_minor=a_protocol_minor;};

		uint8_t getSystem_major(){return system_major;};
		void setSystem_major(uint8_t a_system_major){system_major=a_system_major;};

		uint8_t getSystem_minor(){return system_minor;};
		void setSystem_minor(uint8_t a_system_minor){system_minor=a_system_minor;};

		uint32_t getMax_package_size(){return max_package_size;};
		void setMax_package_size(uint32_t a_max_package_size){max_package_size=a_max_package_size;};

		struct FeaturesMap getFeatures(){return features;};
		void setFeatures(struct FeaturesMap a_features){features=a_features;};

		struct Auth_methodsMap getAuth_methods(){return auth_methods;};
		void setAuth_methods(struct Auth_methodsMap a_auth_methods){auth_methods=a_auth_methods;};

		CharArray* getSalt(){return salt;};
		void setSalt(CharArray* a_salt){salt=a_salt;};


		uint8_t getPackageType(){return ID_W_s_helloPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define W_S_HELLOPACKAGE_H_*/
