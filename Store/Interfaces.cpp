#include "NamedItems.h"

namespace Store
{
	Interfaces::Interfaces() 
	{
	    ec = new ErrorConsole("Store: Interfaces");
	    *ec << "Interfaces constructed";
	    STORE_IX_INITIALPAGECOUNT = STORE_IXC_INITIALPAGECOUNT;
	    STORE_IX_NAMEMAXLEN = STORE_IXC_NAMEMAXLEN;
	    STORE_IX_NULLVALUE = STORE_IXC_NULLVALUE;
	    STORE_FILE_ = STORE_FILE_INTERFACES;
	    STORE_PAGE_HEADER = STORE_PAGE_INTERFACESHEADER;
	    STORE_PAGE_PAGE = STORE_PAGE_INTERFACESPAGE;    
	}

	Interfaces::~Interfaces() 
	{
	    ec->printf("Interfaces destructor called, my last message\n");
	    delete(this->ec);
	}

	int Interfaces::createEntry(int logicalID, const char* name, const char* objectName, int transactionID, int transactionTimeStamp, int& size_needed, char*& entry_buf) 
	{	
		ix_entry* entry = 0;
		int name_len = strlen(name);
		int objectName_len = strlen(objectName);
		if ((objectName_len > STORE_IX_NAMEMAXLEN) || (name_len > STORE_IX_NAMEMAXLEN))
			return 1;	
		size_needed = sizeof(ix_entry) + name_len + 1 + objectName_len + 1 ;
		entry_buf = new char[size_needed];
		memset(entry_buf, 0, size_needed);
		entry = (ix_entry*) entry_buf;
		entry->size = size_needed;
		entry->l_id = logicalID;
		entry->cur_tran = transactionID;
		entry->add_t = transactionTimeStamp;
		entry->del_t = STORE_IX_NULLVALUE;
		memcpy(entry->name, name, name_len);
		memcpy(entry->name + name_len + 1, objectName, objectName_len);
		return 0;
	}

	
	int Interfaces::addInterface(int lid, const char* name, const char* objectName, int tid, int tTimeStamp) 
	{
	    char *entryBuf;
	    int size = 0;
	    int err;
	    
	    ec->printf("Interfaces::addInterface creating entry\n");
	    
	    if ((err=createEntry(lid, name, objectName, tid, tTimeStamp, size, entryBuf))!=0)
		return err;
		
	    ec->printf("Interfaces::addInterface Entry created\n");	
		
	    if ((err=addItem(size, entryBuf))!=0)
		return err;
	    
	    ec->printf("Interfaces::addInterface item added\n");	
	    
	    if (entryBuf != NULL) 
		delete entryBuf;
	
	    return 0;
	}
    /*
	vector<int>* Classes::getClassByInvariant(const char* invariantName, int transactionID, int transactionTimeStamp) 
	{
		//return getItemsByAnything(findByName, invariantName, transactionID, transactionTimeStamp);
		return getItemsByAnything(findByInvariantName, invariantName, transactionID, transactionTimeStamp);
	}
    */	
}

