#ifndef __STORE_DBPHYSICALID_H__
#define __STORE_DBPHYSICALID_H__
namespace Store { class DBPhysicalID; }

#include "Store.h"
#include "Struct.h"

using namespace std;

namespace Store
{
	class DBPhysicalID //: PhysicalID // base class 'PhysicalID' is no more
	{
	public:
		DBPhysicalID(physical_id pid);
		virtual unsigned short getFile() const;
		virtual unsigned int getPage() const;
		virtual unsigned short getOffset() const;		
		virtual long long getLockAddress() const;
		virtual ~DBPhysicalID(){};

	private:
		unsigned short file;
		unsigned int page;
		unsigned short offset;
	};	
	
}

#endif

