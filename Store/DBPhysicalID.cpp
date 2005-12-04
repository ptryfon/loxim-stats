#include "DBPhysicalID.h"

using namespace std;

namespace Store
{
	unsigned short DBPhysicalID::getFile() const
	{
		return 0;
	};

	unsigned int DBPhysicalID::getPage() const
	{
		return 0;
	};

	unsigned short DBPhysicalID::getOffset() const
	{
		return 0;
	};
	
}

