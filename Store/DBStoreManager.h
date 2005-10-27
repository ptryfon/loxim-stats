#ifndef __DBPHYSICALID_H__
#define __DBPHYSICALID_H__

#include "Store.h"

using namespace std;

namespace Store
{
	class DBStoreManager : public StoreManager
	{
	public:
		virtual ObjectPointer* getObject(TransactionID* tid, LogicalID* lid, ObjectPointer::AccessMode mode);
		virtual ObjectPointer* createObject(TransactionID* tid, string name, DataValue* value);
		virtual void deleteObject(TransactionID* tid, ObjectPointer* object);

		virtual vector<ObjectPointer*>* getRoots(TransactionID* tid);
		virtual vector<ObjectPointer*>* getRoots(TransactionID* tid, string name);
		virtual void addRoot(TransactionID* tid, ObjectPointer* object);
		virtual void removeRoot(TransactionID* tid, ObjectPointer* object);

		virtual void abortTransaction(TransactionID* tid);
		virtual void commitTransaction(TransactionID* tid);

		virtual DataValue* createIntValue(int value);
		virtual DataValue* createDoubleValue(double value);
		virtual DataValue* createStringValue(string value);
		virtual DataValue* createVectorValue(vector<ObjectPointer*>* value);
		virtual DataValue* createPointerValue(ObjectPointer* value);

		virtual LogicalID* logicalIDFromByteArray(unsigned char* lid, int length);
		virtual DataValue* dataValueFromByteArray(unsigned char* value, int length);
	};
}

#endif

