#include <Store/NamedItems.h>

namespace Store
{
	Classes::~Classes()
	{
#ifdef IX_DEBUG
#endif
	};


	int Classes::createEntry(TransactionID* /*tid*/, int /*logicalID*/, const char* /*name*/, int& /*size_needed*/, char*& /*entry_buf*/) {
#ifdef IX_DEBUG
		debug_printf(*ec, "Not possible to create class with this method.");
#endif
		return 1;
	}


	int Classes::createEntry(TransactionID* tid, int logicalID, const char* name, const char* invariantName, int& size_needed, char*& entry_buf) {
		//troche nie mam pomysłu jak tu nie kopiowac z NamedItems::createEntry
		int name_len = 0;
		int invariant_len = 0;
		ixc_entry* entry = 0;

		name_len = strlen(name);
		if (name_len > STORE_IX_NAMEMAXLEN)
			return 1;

		invariant_len = strlen(invariantName);
		if (invariant_len > invaraint_maxlen)
			return 1;

		size_needed = sizeof(ix_entry) + (name_len + 1) + (invariant_len + 1);
		entry_buf = new char[size_needed];
		memset(entry_buf, 0, size_needed);


		entry = (ixc_entry*) entry_buf;
		entry->size = size_needed;
		entry->l_id = logicalID;
		entry->cur_tran = tid->getId();
		entry->add_t = tid->getTimeStamp();
		entry->del_t = STORE_IX_NULLVALUE;
		memcpy(entry->name, name, name_len);
		memcpy(entry->name + (name_len + 1), invariantName, invariant_len);

		return 0;
	}

	int Classes::addClass(TransactionID* tid, int logicalID, const char* name, const char* invariantName) {
#ifdef IX_DEBUG
		debug_printf(*ec, "addClass(logicalID=%i, name=\"%s\", invariantName=\"%s\", transactionID=%i, transactionTimeStamp=%i)\n",
			logicalID, name, invariantName, tid->getId(), tid->getTimeStamp());
#endif
		char* entry_buf;
		int size_needed = 0;
		int errcode = createEntry(tid, logicalID, name, invariantName, size_needed, entry_buf);
		if(errcode != 0) return errcode;
		addItem(tid, size_needed, entry_buf);
		if(entry_buf != NULL){
			delete entry_buf;
		}
		return 0;
	}

}
