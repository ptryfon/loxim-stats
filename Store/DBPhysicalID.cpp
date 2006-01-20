#include "DBPhysicalID.h"

using namespace std;

namespace Store
{
	unsigned short DBPhysicalID::getFile() const
	{
		return file;
	};

	unsigned int DBPhysicalID::getPage() const
	{
		return page;
	};

	unsigned short DBPhysicalID::getOffset() const
	{
		return offset;
	};
	
}

