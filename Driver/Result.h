#ifndef RESULT_H_
#define RESULT_H_

#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

class Result
{
public:
	Result() {};
	virtual void toStream(ostream&) const;
	virtual ~Result() {};
	
	friend ostream& operator<<(ostream&, Result&);	
};//class Result


class ResultCollection : public Result 
{
public:
	ResultCollection() {};
	virtual int     count()          = 0;
	virtual void    add(Result* val) = 0;
	virtual void    replace(int i, Result* val) = 0;
	virtual Result* get(int i)       = 0;
	//virtual void    remove(int i)    = 0;
	//Container::iterator getIterator();
	//Any idea how to say simply "iterator" here. That doesn't work.
};//class ResultCollection : Result 


class ResultBag : public ResultCollection
{
private:	
	vector<Result*> bag;
public:
	ResultBag(unsigned long size) {        }
	int     count()           { return bag.size();      }
	void    add(Result* val)  { bag.push_back(val);     }
	void    replace(int i, Result* val) { bag[i] = val; }
	Result* get(int i)        { return bag[i];          }	
	//void    remove(int i)     {                         }
	virtual ~ResultBag() {};
};//class ResultBag : ResultCollection


class ResultSequence : public ResultCollection
{
public:
	ResultSequence() {};
	virtual ~ResultSequence() {};
};//class ResultSequence : ResultCollection


class ResultSingle : public Result
{
public:
	ResultSingle() {}
	virtual ~ResultSingle() {};
};//class ResultSingle


class ResultReference : public ResultSingle
{
private:
	int   value;
	char* ch_value;
	
public:
	ResultReference(int val);
	ResultReference(char* val)   { ch_value = val;  }
	void   setValue(int val)     { value = val;     }
	int    getValue()		     { return value; }
	void   setChValue(char* val) { ch_value = val;  }
	char*  getChValue()          { return ch_value; }
	string toString();
	virtual ~ResultReference()   { if (ch_value!=NULL) delete[] ch_value; }
};//class ResultReference : ResultSingle


class ResultVoid : public Result
{
public:
	ResultVoid() {};
	virtual ~ResultVoid() {};
};//class ResultVoid : Result

#endif /*RESULT_H_*/
