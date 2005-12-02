#ifndef _QUERYRESULT_H_
#define _QUERYRESULT_H_

#include <string>
#include <vector>
#include "Store/Store.h"

using namespace std;

namespace QExecutor {

class QueryResult
{
public:
	enum QueryResultType {
		QRESULT   = 0,
		QSEQUENCE = 1,
		QBAG      = 2,
		QSTRUCT   = 3,
		QBINDER   = 4,
		QSTRING   = 5,
		QINT      = 6,
		QDOUBLE   = 7,
		QBOOL     = 8,
		QREFERENCE= 9,
		QNOTHING  =10
		};
		
	//virtual bool operator==(QueryResult& r)=0;
	virtual QueryResult* clone()=0;
	
	virtual int type();
	virtual bool collection()=0;
	virtual bool isEmpty()=0;
	virtual int size()=0;
	virtual void addResult(QueryResult *r) {};
	virtual int getResult(QueryResult *&r) { return 0; };
};


class QuerySequenceResult : public QueryResult
{
protected:
	vector<QueryResult*> seq;
	vector<QueryResult*>::iterator it;
public:
	QuerySequenceResult ();
	QuerySequenceResult (unsigned int size);
	QuerySequenceResult (vector<QueryResult*> s);
	//virtual bool operator==(QueryResult& r){ return false; };
	//virtual void operator+=(QueryResult& r){};
	//virtual QueryResult& operator[](int p){ return *(this->clone());};
	QueryResult* clone();
	virtual ~QuerySequenceResult() {
		if ((seq.size()) > 0) {
			for (unsigned int i = 0; i < (seq.size()); i++ ) { delete (seq.at(i)); };
		}; 
	};
	int type();
	bool collection();
	bool isEmpty();
	int size();
	void addResult(QueryResult *r);
	int getResult(QueryResult *&r);
};

class QueryBagResult : public QueryResult
{
protected:
	vector<QueryResult*> bag;
	vector<QueryResult*>::iterator it;
public:
	QueryBagResult ();
	QueryBagResult (unsigned int size);
	QueryBagResult (vector<QueryResult*> b);
	//virtual bool operator==(QueryResult& r){ return false; };
	//virtual void operator+=(QueryResult& r){};
	QueryResult* clone();
	virtual ~QueryBagResult() {
		if ((bag.size()) > 0) {
			for (unsigned int i = 0; i < (bag.size()); i++ ) { delete (bag.at(i)); };
		}; 
	};
	int type();
	bool collection();
	bool isEmpty();
	int size();
	void addResult(QueryResult *r);
	int getResult(QueryResult *&r);
};


class QueryStructResult : public QueryResult
{
protected:
	vector<QueryResult*> str;
	vector<QueryResult*>::iterator it;
public:
	QueryStructResult ();
	QueryStructResult (unsigned int size);
	QueryStructResult (vector<QueryResult*> s);
	//virtual bool operator==(QueryResult& r){ return false; };
	//virtual void operator+=(QueryResult& r){};
	QueryResult* clone();
	virtual ~QueryStructResult() {
		if ((str.size()) > 0) {
			for (unsigned int i = 0; i < (str.size()); i++ ) { delete (str.at(i)); };
		}; 
	};
	int type();
	bool collection();
	bool isEmpty();
	int size();
	void addResult(QueryResult *r);
	int getResult(QueryResult *&r);
};


class QueryBinderResult : public QueryResult
{
protected:
	string name;
	QueryResult* item;
public:
	QueryBinderResult(string n, QueryResult* r);
	//virtual bool operator==(QueryResult& r){ return false; };
	QueryResult* clone();
	virtual ~QueryBinderResult() { if (item != NULL) delete item; };
	string getName();
	void setName(string n);
	QueryResult* getItem();
	void setItem(QueryResult* r);
	int type();
	bool collection();
	bool isEmpty();
	int size();
};


class QueryStringResult : public QueryResult
{
protected:
	string value;
public:
	QueryStringResult(string  v);
	//virtual bool operator==(QueryResult& r){ return false; };
	QueryResult* clone();
	virtual ~QueryStringResult() {};
	string getValue();
	void setValue(string v);
	int type();
	bool collection();
	bool isEmpty();
	int size();
};


class QueryIntResult : public QueryResult
{
protected:
	int value;
public:
	QueryIntResult(int  v);
	//virtual bool operator==(QueryResult& r){ return false; };
	QueryResult* clone();
	virtual ~QueryIntResult() {};
	int getValue();
	void setValue(int v);
	int type();
	bool collection();
	bool isEmpty();
	int size();
};


class QueryDoubleResult : public QueryResult
{
protected:
	double value;
public:
	QueryDoubleResult(double v);
	//virtual bool operator==(QueryResult& r){ return false; };
	QueryResult* clone();
	virtual ~QueryDoubleResult() {};
	double getValue();
	void setValue(double v);
	int type();
	bool collection();
	bool isEmpty();
	int size();
};

class QueryBoolResult : public QueryResult
{
protected:
	bool value;
public:
	QueryBoolResult(bool v);
	//virtual bool operator==(QueryResult& r){ return false; };
	QueryResult* clone();
	virtual ~QueryBoolResult() {};
	bool getValue();
	void setValue(bool v);
	int type();
	bool collection();
	bool isEmpty();
	int size();
};


class QueryReferenceResult : public QueryResult
{
protected:
	LogicalID *value;
public:
	QueryReferenceResult(LogicalID* v);
	//virtual bool operator==(QueryResult& r){ return false; };
	QueryResult* clone();
	virtual ~QueryReferenceResult() { if (value != NULL) delete value; };
	LogicalID* getValue();
	void setValue(LogicalID* v);
	int type();
	bool collection();
	bool isEmpty();
	int size();
};

/* You get this, as a result of non-select query like create, insert etc. */
class QueryNothingResult : public QueryResult
{
public:
	QueryNothingResult();
	QueryResult* clone();
	virtual ~QueryNothingResult() {};
	int type();
	bool collection();
	bool isEmpty();
	int size();
};

}

#endif  /* _QUERYRESULT_H_ */
