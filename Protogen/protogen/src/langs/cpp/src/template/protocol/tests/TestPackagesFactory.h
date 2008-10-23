#ifndef __TESTPACKAGESFACTORY_H
#define __TESTPACKAGESFACTORY_H

#include <stdio.h>
#include <stdlib.h>

#include "../ptools/Package.h"

namespace protocol{
	class TestPackagesFactory
	{
		public: 
			static Package* createPackage(int nr);
	};
};

#endif
