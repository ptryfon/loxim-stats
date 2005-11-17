#ifndef RESULTCOLLECTION_H_
#define RESULTCOLLECTION_H_

#include <iterator>
#include "Result.h"
#include "ResultSingle.h"


class ResultCollection : Result 
{
public:
	ResultCollection() {};
		
	virtual int  count();

	virtual void add(Result* val);

	virtual void replace(int i, Result* val);

	virtual void remove(int i);

	virtual Result& get(int i);

	//Container::iterator getIterator();

	/**
	 * Makes collection flat. Of course, it has to be singleton
	 */
	virtual ResultSingle toResultSingle();
	
	virtual ~ResultCollection();
	
};

#endif /*RESULTCOLLECTION_H_*/
