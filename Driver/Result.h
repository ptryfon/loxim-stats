#ifndef RESULT_H_
#define RESULT_H_

#include <iostream>

using namespace std;

class Result
{
public:
	Result() {};
	virtual void toStream(ostream&) const;
	virtual ~Result() {};
	
	friend ostream& operator<<(ostream&, Result&);	
};

#endif /*RESULT_H_*/
