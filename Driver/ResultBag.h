#ifndef RESULTBAG_H_
#define RESULTBAG_H_

#include <vector>
#include "Result.h"
#include "ResultCollection.h"


using namespace std;

class ResultBag : ResultCollection
{
private:	
//	vector <Result> bag;
	
public:
	ResultBag(unsigned long size);

	
	virtual ~ResultBag();
};

#endif /*RESULTBAG_H_*/
