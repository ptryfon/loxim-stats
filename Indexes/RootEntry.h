#ifndef __ROOTENTRY_H__
#define __ROOTENTRY_H__

#include "Store/Store.h"
#include "const.h"

namespace Indexes {

		struct RootEntry {
			tid_t modyfierTransactionId;
			ts_t addTime;
			ts_t delTime;
			lid_t logicalID;
		//	int value;
	
			Store::LogicalID* getLogicalID();
			RootEntry* clone();
	
			RootEntry(ts_t add_timestamp, ts_t del_t, tid_t cur_tran, lid_t l_id);
			//int addEntry(ObjectPointer* op, TransactionID* tid) {return 0;};
			
			string toString();
		};
		
		
};

#endif /*__ROOTENTRY_H__*/
