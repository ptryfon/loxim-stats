#include <Store/NamedItems.h>

	NamedRoots::~NamedRoots()
	{
#ifdef IX_DEBUG
		delete(this->ec);
#endif
	};
