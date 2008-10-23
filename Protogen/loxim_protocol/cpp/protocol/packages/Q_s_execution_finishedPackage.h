#ifndef Q_S_EXECUTION_FINISHEDPACKAGE_H_
#define Q_S_EXECUTION_FINISHEDPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_Q_s_execution_finishedPackage 68

class Q_s_execution_finishedPackage : public Package
{
	private:
		int64_t modAtomPointerCnt;
		int64_t delCnt;
		int64_t newRootsCnt;
		int64_t insertsCnt;
	public:
		Q_s_execution_finishedPackage();
		~Q_s_execution_finishedPackage();
		Q_s_execution_finishedPackage(
			int64_t a_modAtomPointerCnt,\
			int64_t a_delCnt,\
			int64_t a_newRootsCnt,\
			int64_t a_insertsCnt
			);

		virtual bool equals(Package* _package);

		int64_t getModAtomPointerCnt(){return modAtomPointerCnt;};
		void setModAtomPointerCnt(int64_t a_modAtomPointerCnt){modAtomPointerCnt=a_modAtomPointerCnt;};

		int64_t getDelCnt(){return delCnt;};
		void setDelCnt(int64_t a_delCnt){delCnt=a_delCnt;};

		int64_t getNewRootsCnt(){return newRootsCnt;};
		void setNewRootsCnt(int64_t a_newRootsCnt){newRootsCnt=a_newRootsCnt;};

		int64_t getInsertsCnt(){return insertsCnt;};
		void setInsertsCnt(int64_t a_insertsCnt){insertsCnt=a_insertsCnt;};


		uint8_t getPackageType(){return ID_Q_s_execution_finishedPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define Q_S_EXECUTION_FINISHEDPACKAGE_H_*/
