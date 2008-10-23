#ifndef V_SC_SENDVALUESPACKAGE_H_
#define V_SC_SENDVALUESPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_V_sc_sendvaluesPackage 32

class V_sc_sendvaluesPackage : public Package
{
	private:
		int64_t rootValueId;
		int64_t oBundlesCount;
		bool bnull_oBundlesCount;
		int64_t oBidCount;
		bool bnull_oBidCount;
		int64_t pVidCount;
		bool bnull_pVidCount;
	public:
		V_sc_sendvaluesPackage();
		~V_sc_sendvaluesPackage();
		V_sc_sendvaluesPackage(
			int64_t a_rootValueId,\
			int64_t a_oBundlesCount,\
			int64_t a_oBidCount,\
			int64_t a_pVidCount
			,bool a_bnull_oBundlesCount=false
			,bool a_bnull_oBidCount=false
			,bool a_bnull_pVidCount=false
			);

		virtual bool equals(Package* _package);

		int64_t getRootValueId(){return rootValueId;};
		void setRootValueId(int64_t a_rootValueId){rootValueId=a_rootValueId;};

		int64_t getOBundlesCount(){return oBundlesCount;};
		bool getBnull_oBundlesCount(){return bnull_oBundlesCount;};
		void setOBundlesCount(int64_t a_oBundlesCount){oBundlesCount=a_oBundlesCount;};
		void setBnull_oBundlesCount(bool a_bnull_oBundlesCount){bnull_oBundlesCount=a_bnull_oBundlesCount;};

		int64_t getOBidCount(){return oBidCount;};
		bool getBnull_oBidCount(){return bnull_oBidCount;};
		void setOBidCount(int64_t a_oBidCount){oBidCount=a_oBidCount;};
		void setBnull_oBidCount(bool a_bnull_oBidCount){bnull_oBidCount=a_bnull_oBidCount;};

		int64_t getPVidCount(){return pVidCount;};
		bool getBnull_pVidCount(){return bnull_pVidCount;};
		void setPVidCount(int64_t a_pVidCount){pVidCount=a_pVidCount;};
		void setBnull_pVidCount(bool a_bnull_pVidCount){bnull_pVidCount=a_bnull_pVidCount;};


		uint8_t getPackageType(){return ID_V_sc_sendvaluesPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define V_SC_SENDVALUESPACKAGE_H_*/
