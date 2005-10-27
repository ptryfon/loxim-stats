#include "DBPhysicalID.h"

using namespace std;

namespace Store
{
	unsigned short DBPhysicalID::getFile()
	{
		return 0;
	};

	unsigned int DBPhysicalID::getPage()
	{
		return 0;
	};

	unsigned short DBPhysicalID::getOffset()
	{
		return 0;
	};
}

