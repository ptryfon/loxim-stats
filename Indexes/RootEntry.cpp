#include "RootEntry.h"
#include "Store/DBLogicalID.h"
#include "ToStringBuilder.h"
#include <sstream>

namespace Indexes {

 	RootEntry::RootEntry(ts_t add_timestamp, ts_t del_t, tid_t cur_tran, lid_t l_id): 
		modyfierTransactionId(cur_tran), addTime(add_timestamp), delTime(del_t), logicalID(l_id)  {};

	RootEntry* RootEntry::clone() {
		return new RootEntry(addTime, delTime, modyfierTransactionId, logicalID);
	}

	LogicalID* RootEntry::getLogicalID() {
		return new DBLogicalID(logicalID);
	}
	
	/*
	string RootEntry::toString() {
		stringstream temp;
		temp << "RootEntry: @" << this << "[modifierTid: " << modyfierTransactionId <<
										", logicalID: " << logicalID << "]";  
		return temp.str();
	}
	*/
	string RootEntry::toString() {
		return ToStringBuilder("RootEntry", this)
			.append("modifierTid", modyfierTransactionId)
			.append("addTime", addTime)
			.append("delTime", delTime)
			.append("logicalID", logicalID)
			.toString();
	}
}
