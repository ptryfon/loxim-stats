#ifndef __STORE_MISC_H_
#define __STORE_MISC_H_

#include "Store.h"
#include <iostream>
using namespace std;

namespace Store
{

	class Misc
	{
	public:
		Misc(){lastlid = 0;};
	   vector <ObjectPointer*> vect;
		vector <ObjectPointer*> roots;
		int lastlid;
	};


};

#endif // __STORE_MISC_H__


