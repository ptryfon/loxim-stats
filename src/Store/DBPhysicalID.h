#ifndef __STORE_DBPHYSICALID_H__
#define __STORE_DBPHYSICALID_H__

#include <Store/Struct.h>

namespace Store
{
	class DBPhysicalID //: PhysicalID // base class 'PhysicalID' is no more
	{
	public:
		DBPhysicalID(physical_id pid);
		unsigned short getFile() const;
		unsigned int getPage() const;
		unsigned short getOffset() const;		
		long long getLockAddress() const;
		~DBPhysicalID(){};

	private:
		unsigned short file;
		unsigned int page;
		unsigned short offset;
	};	
	
}

#endif

