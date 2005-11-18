#ifndef __STORE_DBPHYSICALID_H__
#define __STORE_DBPHYSICALID_H__

#include "Store.h"

using namespace std;

namespace Store
{
	class DBPhysicalID : public PhysicalID
	{
	public:
		virtual unsigned short getFile();
		virtual unsigned int getPage();
		virtual unsigned short getOffset();
	};
}

#endif

