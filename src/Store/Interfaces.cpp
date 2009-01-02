#include <Store/NamedItems.h>

namespace Store
{
	Interfaces::Interfaces()
	{
	    ec = &ErrorConsole::get_instance(EC_STORE_INTERFACES);
	    debug_print(*ec,  "Interfaces constructed");
	    STORE_IX_INITIALPAGECOUNT = STORE_IXC_INITIALPAGECOUNT;
	    STORE_IX_NAMEMAXLEN = STORE_IXC_NAMEMAXLEN;
	    STORE_IX_NULLVALUE = STORE_IXC_NULLVALUE;
	    STORE_FILE_ = STORE_FILE_INTERFACES;
	    STORE_PAGE_HEADER = STORE_PAGE_INTERFACESHEADER;
	    STORE_PAGE_PAGE = STORE_PAGE_INTERFACESPAGE;
	}

	Interfaces::~Interfaces()
	{
	    debug_printf(*ec, "Interfaces destructor called, my last message\n");
	}

	int Interfaces::getInterfaceBindForObjectName(TransactionID* tid, const string &oName, string &interfaceName, string &bindName)
	{
	    vector<const ix_entry*> *entriesVec = new vector<const ix_entry*>(); 
	    getInterfaceByObjectName(tid, oName.c_str(), entriesVec);
	    
	    if (entriesVec->size() > 1)
	    {	//TODO
		debug_printf(*ec, "ERROR in getInterfaceBindForObjectName\n");
		return -1;
	    }
	    else if (entriesVec->size() == 1)
	    {	
		const ixi_entry *en = static_cast<const ixi_entry *>(entriesVec->at(0));
	    
		interfaceName = en->getName();
		bindName = en->getBindName();
		debug_printf(*ec, "Found entry for object name %s: int - %s, bind - %s  \n", oName.c_str(), interfaceName.c_str(), bindName.c_str());
		return 0;
	    }
	    else //No interface
		return ENoInterfaceFound | ErrStore;
	}
	
	int Interfaces::getInterfaceBindForName(TransactionID* tid, const string &interfaceName, string &oName, string &bindName)
	{
	    vector<const ix_entry*> *entriesVec = new vector<const ix_entry*>(); 
	    getByNameWithEntries(tid, interfaceName.c_str(), entriesVec);
	    
	    if (entriesVec->size() > 1)
	    {	//TODO
		debug_printf(*ec, "ERROR in getInterfaceBindForName\n");
		return -1;
	    }
	    else if (entriesVec->size() == 1)
	    {	
		const ixi_entry *en = static_cast<const ixi_entry *>(entriesVec->at(0));
	    
		oName = en->getObjectName();
		bindName = en->getBindName();
		debug_printf(*ec, "Found entry for interface name %s: obj - %s, bind - %s  \n", interfaceName.c_str(), oName.c_str(), bindName.c_str());
		return 0;    
	    }
	    else 
		return ENoInterfaceFound | ErrStore;
	} 

	int Interfaces::createEntry(TransactionID* tid, int logicalID, const char* name, const char* objectName, int& size_needed, char*& entry_buf, const char* bindName)
	{
		ix_entry* entry = 0;
		int name_len = strlen(name);
		int objectName_len = strlen(objectName);
		bool isBindName = (bindName && (strlen(bindName) > 0));
		
		int bindName_len = isBindName ? strlen(bindName) : 0;
		if ((objectName_len > STORE_IX_NAMEMAXLEN) || (name_len > STORE_IX_NAMEMAXLEN) || (bindName_len > STORE_IX_NAMEMAXLEN))
			return 1;
		size_needed = sizeof(ix_entry) + name_len + 1 + objectName_len + 1 + bindName_len + 1;
		entry_buf = new char[size_needed];
		memset(entry_buf, 0, size_needed);
		entry = (ix_entry*) entry_buf;
		entry->size = size_needed;
		entry->l_id = logicalID;
		entry->cur_tran = tid->getId();
		entry->add_t = tid->getTimeStamp();
		entry->del_t = STORE_IX_NULLVALUE;
		memcpy(entry->name, name, name_len);
		memcpy(entry->name + name_len + 1, objectName, objectName_len);
		if (isBindName) memcpy(entry->name + name_len + 1 + objectName_len + 1, bindName, bindName_len);
		return 0;
	}
	
	int Interfaces::bindInterface(TransactionID* tid, const string& name, const string& bindName)
	{
	    string objectName, oldBindName;
	    int errcode = getInterfaceBindForName(tid, name, objectName, oldBindName); 
	    if (!errcode)
	    {
		debug_printf(*ec, "Interfaces::bindInterface binding interface with name:%s, objectName:%s with %s\n", name.c_str(), objectName.c_str(), bindName.c_str());
	    
		vector<int>* lids = getItems(tid, name.c_str());
		if (lids->size() == 1)
		{
		    int lid = lids->at(0);
		    removeItem(tid, lid);
		    return addInterface(tid, lid, name, objectName, bindName);	
		}
	    }
	    return ENoInterfaceFound | ErrStore;
	}

	int Interfaces::addInterface(TransactionID* tid, int lid, const string& name, const string& objectName, const string& bindName)
	{
	    char *entryBuf;
	    int size = 0;
	    int err;

	    debug_printf(*ec, "Interfaces::addInterface creating entry\n");

	    if ((err=createEntry(tid, lid, name.c_str(), objectName.c_str(), size, entryBuf, bindName.c_str()))!=0)
		return err;

	    debug_printf(*ec, "Interfaces::addInterface Entry created\n");

	    if ((err=addItem(tid, size, entryBuf))!=0)
		return err;

	    debug_printf(*ec, "Interfaces::addInterface item added\n");

	    if (entryBuf != NULL)
		delete entryBuf;

	    return 0;
	}
}

