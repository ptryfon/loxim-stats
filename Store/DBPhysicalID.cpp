#include "DBPhysicalID.h"

using namespace std;

namespace Store
{
	DBPhysicalID::DBPhysicalID(physical_id pid)
	{
		file = pid.file_id;
		page = pid.page_id;
		offset = pid.offset;
	};
	
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

