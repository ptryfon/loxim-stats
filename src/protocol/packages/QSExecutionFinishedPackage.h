#ifndef QSEXECUTIONFINISHEDPACKAGE_H_
#define QSEXECUTIONFINISHEDPACKAGE_H_

#include <stdint.h> 
#include <protocol/ptools/Package.h>

#define ID_QSExecutionFinishedPackage 70

namespace protocol{

class QSExecutionFinishedPackage:public Package
{
	private:
		uint64_t modAtomPointerCnt;
		uint64_t delCnt;
		uint64_t newRootsCnt;
		uint64_t insertsCnt;	
		bool bnull_modAtomPointerCnt;
		bool bnull_delCnt;
		bool bnull_newRootsCnt;
		bool bnull_insertsCnt;	
	
	public: 
		QSExecutionFinishedPackage();
		QSExecutionFinishedPackage(
			uint64_t a_modAtomPointerCnt,
			bool a_bnull_modAtomPointerCnt,
			uint64_t a_delCnt,
			bool a_bnull_delCnt,
			uint64_t a_newRootsCnt,
			bool a_bnull_newRootsCnt,
			uint64_t a_insertsCnt,
			bool a_bnull_insertsCnt
		);
			
		uint8_t getPackageType(){return ID_QSExecutionFinishedPackage;};
		
		uint64_t getModAtomPointerCnt(){return modAtomPointerCnt;};
		uint64_t getDelCnt(){return delCnt;};
		uint64_t getNewRootsCnt(){return newRootsCnt;};
		uint64_t getInsertsCnt(){return insertsCnt;};
		
		bool getBnull_modAtomPointerCnt(){return bnull_modAtomPointerCnt;};
		bool getBnull_delCnt(){return bnull_delCnt;};
		bool getBnull_newRootsCnt(){return bnull_newRootsCnt;};
		bool getBnull_insertsCnt(){return bnull_insertsCnt;};
		
		virtual bool equals(Package* package);
		
	protected: 
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
 	
};
}
#endif /*QSEXECUTIONFINISHEDPACKAGE_H_*/
