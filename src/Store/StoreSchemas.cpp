#include <Store/NamedItems.h>

namespace Store
{
	StoreSchemas::StoreSchemas()
	{
	    ec = new ErrorConsole("Store: Schemas");
	    *ec << "StoreSchemas constructed";
	    STORE_IX_INITIALPAGECOUNT = STORE_IXC_INITIALPAGECOUNT;
	    STORE_IX_NAMEMAXLEN = STORE_IXC_NAMEMAXLEN;
	    STORE_IX_NULLVALUE = STORE_IXC_NULLVALUE;
	    STORE_FILE_ = STORE_FILE_SCHEMAS;
	    STORE_PAGE_HEADER = STORE_PAGE_SCHEMASHEADER;
	    STORE_PAGE_PAGE = STORE_PAGE_SCHEMASPAGE;
	}
}