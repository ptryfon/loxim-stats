#ifndef RESULTREFERENCE_H_
#define RESULTREFERENCE_H_

#include <string>
#include "ResultSingle.h"

using namespace std;

class ResultReference : ResultSingle
{
private:
	int value;
	char* ch_value;
	
public:
	ResultReference(int val);
	ResultReference(char* val);
	void   setValue(int val)     { value = val;  }
	int    getValue()		     { return value; }
	void   setChValue(char* val) { ch_value = val; }
	char*  getChValue()          { return ch_value; }
	string toString();
	virtual ~ResultReference() { if (ch_value!=NULL) delete[] ch_value; }
};

#endif /*RESULTREFERENCE_H_*/
