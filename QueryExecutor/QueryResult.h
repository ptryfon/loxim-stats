#ifndef _QUERYRESULT_H_
#define _QUERYRESULT_H_

#include <stdio.h>
#include <string>
#include <vector>
   
#include "TransactionManager/Transaction.h"
#include "Store/Store.h"
#include "Store/DBDataValue.h"
#include "Store/DBLogicalID.h"
#include "QueryParser/QueryParser.h"
#include "QueryParser/TreeNode.h"
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
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
		
	virtual QueryResult* clone()=0;
	
	virtual int type();
	virtual bool collection()=0;
	virtual bool isEmpty()=0;
	virtual unsigned int size()=0;
	virtual void addResult(QueryResult *r) {};
	virtual int getResult(QueryResult *&r) { return 0; };
	virtual bool equal(QueryResult *r)=0;
	virtual bool not_equal(QueryResult *r)=0;
	virtual bool greater_than(QueryResult *r)=0;
	virtual bool less_than(QueryResult *r)=0;
	virtual bool greater_eq(QueryResult *r)=0;
	virtual bool less_eq(QueryResult *r)=0;
	virtual int nested(Transaction *tr, QueryResult *&r)=0;
	virtual bool isBool()=0;
	virtual int getBoolValue(bool &b)=0;
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
	QueryResult* clone();
	virtual ~QuerySequenceResult() {
		if ((seq.size()) > 0) {
			for (unsigned int i = 0; i < (seq.size()); i++ ) { delete (seq.at(i)); };
		}; 
	};
	int type();
	bool collection();
	bool isEmpty();
	unsigned int size();
	void addResult(QueryResult *r);
	int getResult(QueryResult *&r);
	int at(unsigned int i, QueryResult *&r);
	bool equal(QueryResult *r);
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r) { return false; };
	bool less_than(QueryResult *r) { return false; };
	bool greater_eq(QueryResult *r) { return false; };
	bool less_eq(QueryResult *r) { return false; };
	int nested(Transaction *tr, QueryResult *&r);
	bool isBool();
	int getBoolValue(bool &b);
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
	QueryResult* clone();
	virtual ~QueryBagResult() {
		if ((bag.size()) > 0) {
			for (unsigned int i = 0; i < (bag.size()); i++ ) { delete (bag.at(i)); };
		}; 
	};
	int type();
	bool collection();
	bool isEmpty();
	unsigned int size();
	void addResult(QueryResult *r);
	int getResult(QueryResult *&r);
	int at(unsigned int i, QueryResult *&r);
	bool equal(QueryResult *r);
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r) { return false; };
	bool less_than(QueryResult *r) { return false; };
	bool greater_eq(QueryResult *r) { return false; };
	bool less_eq(QueryResult *r) { return false; };
	int nested(Transaction *tr, QueryResult *&r);
	bool isBool();
	int getBoolValue(bool &b);
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
	QueryResult* clone();
	virtual ~QueryStructResult() {
		if ((str.size()) > 0) {
			for (unsigned int i = 0; i < (str.size()); i++ ) { delete (str.at(i)); };
		}; 
	};
	int type();
	bool collection();
	bool isEmpty();
	unsigned int size();
	void addResult(QueryResult *r);
	int getResult(QueryResult *&r);
	int at(unsigned int i, QueryResult *&r);
	bool equal(QueryResult *r);
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r) { return false; };
	bool less_than(QueryResult *r) { return false; };
	bool greater_eq(QueryResult *r) { return false; };
	bool less_eq(QueryResult *r) { return false; };
	int nested(Transaction *tr, QueryResult *&r);
	bool isBool();
	int getBoolValue(bool &b);
};


class QueryBinderResult : public QueryResult
{
protected:
	string name;
	QueryResult* item;
public:
	QueryBinderResult();
	QueryBinderResult(string n, QueryResult* r);
	QueryResult* clone();
	virtual ~QueryBinderResult() { if (item != NULL) delete item; };
	string getName();
	void setName(string n);
	QueryResult* getItem();
	void setItem(QueryResult* r);
	int type();
	bool collection();
	bool isEmpty();
	unsigned int size();
	bool equal(QueryResult *r);
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r) { return false; };
	bool less_than(QueryResult *r) { return false; };
	bool greater_eq(QueryResult *r) { return false; };
	bool less_eq(QueryResult *r) { return false; };
	int nested(Transaction *tr, QueryResult *&r);
	bool isBool();
	int getBoolValue(bool &b);
};


class QueryStringResult : public QueryResult
{
protected:
	string value;
public:
	QueryStringResult();
	QueryStringResult(string  v);
	QueryResult* clone();
	virtual ~QueryStringResult() {};
	string getValue();
	void setValue(string v);
	int type();
	bool collection();
	bool isEmpty();
	unsigned int size();
	bool equal(QueryResult *r);
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r);
	bool less_than(QueryResult *r);
	bool greater_eq(QueryResult *r);
	bool less_eq(QueryResult *r);
	int nested(Transaction *tr, QueryResult *&r);
	bool isBool();
	int getBoolValue(bool &b);
};


class QueryIntResult : public QueryResult
{
protected:
	int value;
public:
	QueryIntResult();
	QueryIntResult(int  v);
	QueryResult* clone();
	virtual ~QueryIntResult() {};
	int getValue();
	void setValue(int v);
	int type();
	bool collection();
	bool isEmpty();
	unsigned int size();
	int plus(QueryResult *r, QueryResult *&res);
	int minus(QueryResult *r, QueryResult *&res);
	int times(QueryResult *r, QueryResult *&res);
	int divide_by(QueryResult *r, QueryResult *&res);
	int minus(QueryResult *&res);
	bool equal(QueryResult *r);
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r);
	bool less_than(QueryResult *r);
	bool greater_eq(QueryResult *r);
	bool less_eq(QueryResult *r);
	int nested(Transaction *tr, QueryResult *&r);
	bool isBool();
	int getBoolValue(bool &b);
};


class QueryDoubleResult : public QueryResult
{
protected:
	double value;
public:
	QueryDoubleResult();
	QueryDoubleResult(double v);
	QueryResult* clone();
	virtual ~QueryDoubleResult() {};
	double getValue();
	void setValue(double v);
	int type();
	bool collection();
	bool isEmpty();
	unsigned int size();
	int plus(QueryResult *r, QueryResult *&res);
	int minus(QueryResult *r, QueryResult *&res);
	int times(QueryResult *r, QueryResult *&res);
	int divide_by(QueryResult *r, QueryResult *&res);
	int minus(QueryResult *&res);
	bool equal(QueryResult *r);
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r);
	bool less_than(QueryResult *r);
	bool greater_eq(QueryResult *r);
	bool less_eq(QueryResult *r);
	int nested(Transaction *tr, QueryResult *&r);
	bool isBool();
	int getBoolValue(bool &b);
};

class QueryBoolResult : public QueryResult
{
protected:
	bool value;
public:
	QueryBoolResult();
	QueryBoolResult(bool v);
	QueryResult* clone();
	virtual ~QueryBoolResult() {};
	bool getValue();
	void setValue(bool v);
	int type();
	bool collection();
	bool isEmpty();
	unsigned int size();
	int bool_and(QueryResult *r, QueryResult *&res);
	int bool_or(QueryResult *r, QueryResult *&res);
	int bool_not(QueryResult *&res);
	bool equal(QueryResult *r);
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r);
	bool less_than(QueryResult *r);
	bool greater_eq(QueryResult *r);
	bool less_eq(QueryResult *r);
	int nested(Transaction *tr, QueryResult *&r);
	bool isBool();
	int getBoolValue(bool &b);
};


class QueryReferenceResult : public QueryResult
{
protected:
	LogicalID *value;
public:
	QueryReferenceResult();
	QueryReferenceResult(LogicalID* v);
	QueryResult* clone();
	virtual ~QueryReferenceResult() { if (value != NULL) delete value; };
	LogicalID* getValue();
	void setValue(LogicalID* v);
	int type();
	bool collection();
	bool isEmpty();
	unsigned int size();
	bool equal(QueryResult *r);
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r);
	bool less_than(QueryResult *r);
	bool greater_eq(QueryResult *r);
	bool less_eq(QueryResult *r);
	int nested(Transaction *tr, QueryResult *&r);
	bool isBool();
	int getBoolValue(bool &b);
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
	unsigned int size();
	bool equal(QueryResult *r);
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r) { return false; };
	bool less_than(QueryResult *r) { return false; };
	bool greater_eq(QueryResult *r) { return false; };
	bool less_eq(QueryResult *r) { return false; };
	int nested(Transaction *tr, QueryResult *&r);
	bool isBool();
	int getBoolValue(bool &b);
};

}

#endif  /* _QUERYRESULT_H_ */
