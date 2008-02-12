#ifndef _QUERYRESULT_H_
#define _QUERYRESULT_H_

#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>

#include "../QueryParser/ClassNames.h"
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

namespace Indexes {
	class IndexManager;
}

namespace QExecutor {

	class QueryExecutor;


template<typename T> class Countable {
protected:
	static int count;
public:
	Countable() { count++; };
	Countable(const Countable &) { count++;  };

	virtual ~Countable() { count--; };
	static int getCount()  { return count; } 
};

template<typename T> int Countable<T>::count = 0;

class QueryResult : public Countable<QueryResult>
{
protected:
	ErrorConsole *ec;
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
		QNOTHING  =10,
		QVIRTUAL  =11
		};

	//QueryResult () {};
	//virtual ~QueryResult() {};
	
	virtual QueryResult* clone()=0;

	string getPrefixForLevel( int level, string name ) {
		string result = "";

		if( level > 0 ) {
			for( int i = 0; i < level; i++ ) {
				result += " |";
			}
	
			result += name + "\n";
			for( int i = 0; i < level-1; i++ ) {
				result += " |";
			}
			result += " +-";
		}
		return result;
	}

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
	virtual int nested(Transaction *&tr, QueryExecutor * qe);
	virtual int nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe);
	virtual bool isBool()=0;
	virtual bool isNothing()=0;
	virtual int getBoolValue(bool &b)=0;
	virtual bool isReferenceValue()=0;
	virtual int getReferenceValue(QueryResult *&r)=0;
	virtual bool sorting_less_eq(QueryResult *arg);
	virtual string toString( int level = 0, bool recursive = false, string name = "" ) { return ""; }
};

class QueryContainerResult : public QueryResult
{
	public:
		virtual int at(unsigned int i, QueryResult *&r)=0;
};

class QuerySequenceResult : public QueryContainerResult
{
	friend class Indexes::IndexManager;
protected:
	vector<QueryResult*> seq;
	vector<QueryResult*>::iterator it;
public:
	QuerySequenceResult ();
	QuerySequenceResult (vector<QueryResult*> s);
	QueryResult* clone();
	virtual ~QuerySequenceResult() {
		if ((seq.size()) > 0) {
			for (unsigned int i = 0; i < (seq.size()); i++ ) { delete (seq.at(i)); };
		};
		if (ec != NULL) delete ec;
	};
	int type();
	bool collection();
	bool isEmpty();
	unsigned int size();
	void addResult(QueryResult *r);
	int getResult(QueryResult *&r);
	int at(unsigned int i, QueryResult *&r);
	vector<QueryResult*> getVector();
	int lastElem(QueryResult *&r);
	bool equal(QueryResult *r);
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r);
	bool less_than(QueryResult *r);
	bool greater_eq(QueryResult *r);
	bool less_eq(QueryResult *r);
	int nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
	int sortCollection(QueryResult *r);
	int postSort(QueryResult *&f);
        virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
          string result = getPrefixForLevel( level, name ) + "[Sequence]\n";
          
          if( recursive ) {
            for( unsigned int i = 0; i < seq.size(); i++ ) {
                result += seq[i]->toString( level+1, true, "seq_elem" );
            }
          }

          return result;
        }
};

class QueryBagResult : public QueryContainerResult
{
protected:
	friend class Indexes::IndexManager;
	vector<QueryResult*> bag;
	vector<QueryResult*>::iterator it;
public:
	QueryBagResult ();
	QueryBagResult (vector<QueryResult*> b);
	QueryResult* clone();
	virtual ~QueryBagResult() {
		if ((bag.size()) > 0) {
			for (unsigned int i = 0; i < (bag.size()); i++ ) { delete (bag.at(i)); };
		};
		if (ec != NULL) delete ec;
	};
	int type();
	bool collection();
	bool isEmpty();
	unsigned int size();
	void addResult(QueryResult *r);
	int getResult(QueryResult *&r);
	int at(unsigned int i, QueryResult *&r);
	vector<QueryResult*> getVector();
	int lastElem(QueryResult *&r);
	bool equal(QueryResult *r);
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r);
	bool less_than(QueryResult *r);
	bool greater_eq(QueryResult *r);
	bool less_eq(QueryResult *r);
	int nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
	int divideBag(QueryResult *&left, QueryResult *&right);
	int sortBag(QueryBagResult *&outBag);
        virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
          stringstream c;
          string sizeS;
          c << bag.size(); c >> sizeS;
          string result = getPrefixForLevel( level, name ) + "[Bag] size=" + sizeS + "\n";
          
          if( recursive ) {
            for( unsigned int i = 0; i < bag.size(); i++ ) {
                result += bag[i]->toString( level+1, true, "bag_elem" );
            }
          }

          return result;
        }
};


class QueryStructResult : public QueryContainerResult
{
protected:
	vector<QueryResult*> str;
	vector<QueryResult*>::iterator it;
public:
	QueryStructResult ();
	QueryStructResult (vector<QueryResult*> s);
	QueryStructResult (QueryResult *elem, QueryResult *value);
	QueryResult* clone();
	virtual ~QueryStructResult() {
		if ((str.size()) > 0) {
			for (unsigned int i = 0; i < (str.size()); i++ ) { delete (str.at(i)); };
		};
		if (ec != NULL) delete ec;
	};
	int type();
	bool collection();
	bool isEmpty();
	unsigned int size();
	void addResult(QueryResult *r);
	int getResult(QueryResult *&r);
	int at(unsigned int i, QueryResult *&r);
	int lastElem(QueryResult *&r);
	bool equal(QueryResult *r);
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r);
	bool less_than(QueryResult *r);
	bool greater_eq(QueryResult *r);
	bool less_eq(QueryResult *r);
	int nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
        virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
          string result = getPrefixForLevel( level, name ) + "[Struct]\n";
          
          if( recursive ) {
            for( unsigned int i = 0; i < str.size(); i++ ) {
                result += str[i]->toString( level+1, true, "struct_elem" );
            }
          }

          return result;
        }
        vector<QueryResult*> getVector() { return str; }
        void clear() { str.clear(); }
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
	virtual ~QueryBinderResult() { if (item != NULL) delete item; if (ec != NULL) delete ec; };
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
	bool greater_than(QueryResult *r);
	bool less_than(QueryResult *r);
	bool greater_eq(QueryResult *r);
	bool less_eq(QueryResult *r);
	int nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
        virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
          string result = getPrefixForLevel( level, n ) + "[Binder] name='" + name + "'\n";
          
          if( recursive ) {
            if( item )
              result += item->toString( level+1, true, "item" );
          }

          return result;
        }
};


class QueryStringResult : public QueryResult
{
protected:
	string value;
public:
	QueryStringResult();
	QueryStringResult(string  v);
	QueryResult* clone();
	virtual ~QueryStringResult() { if (ec != NULL) delete ec; };
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
	int nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
        virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
          return getPrefixForLevel( level, n ) + "[String] value='" + value + "'\n";
        }
};


class QueryIntResult : public QueryResult
{
protected:
	int value;
public:
	QueryIntResult();
	QueryIntResult(int  v);
	QueryResult* clone();
	virtual ~QueryIntResult() { if (ec != NULL) delete ec; };
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
	int nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
        virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
          stringstream c;
          string valueS;
          c << value; c >> valueS;

          return getPrefixForLevel( level, n ) + "[Int] value=" + valueS + "\n";
        }
};


class QueryDoubleResult : public QueryResult
{
protected:
	double value;
public:
	QueryDoubleResult();
	QueryDoubleResult(double v);
	QueryResult* clone();
	virtual ~QueryDoubleResult() { if (ec != NULL) delete ec; };
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
	int nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
        virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
          stringstream c;
          string valueS;
          c << value; c >> valueS;

          return getPrefixForLevel( level, n ) + "[Double] value=" + valueS + "\n";
        }
};

class QueryBoolResult : public QueryResult
{
protected:
	bool value;
public:
	QueryBoolResult();
	QueryBoolResult(bool v);
	QueryResult* clone();
	virtual ~QueryBoolResult() { if (ec != NULL) delete ec; };
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
	int nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
        virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
          return getPrefixForLevel( level, n ) + "[Bool] value=" + (value ? "true" : "false" ) + "\n";
        }
};


class QueryReferenceResult : public QueryResult
{
protected:
	LogicalID *value;
	bool refed;
public:
	QueryReferenceResult();
	QueryReferenceResult(LogicalID* v);
	QueryResult* clone();
	virtual ~QueryReferenceResult() { if (value != NULL) delete value; if (ec != NULL) delete ec; };
	LogicalID* getValue();
	void setValue(LogicalID* v);
	bool wasRefed();
	void setRef();
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
	int nested(Transaction *&tr, QueryExecutor * qe);
	//int nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
        virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
          string valueS = value ? value->toString() : "<null>";

          return getPrefixForLevel( level, n ) + "[Ref] refed=" + (refed ? "true" : "false" ) + ", value=" + valueS + "\n";
        }
};


/* You get this, as a result of non-select query like delete or if anb error accurs */
class QueryNothingResult : public QueryResult
{
public:
	QueryNothingResult();
	QueryResult* clone();
	virtual ~QueryNothingResult() { if (ec != NULL) delete ec; };
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
	int nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
        virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
          return getPrefixForLevel( level, n ) + "[Nothing]\n";
        }
};


class QueryVirtualResult : public QueryResult
{
protected:
	
public:
	string vo_name;
	LogicalID *view_def;
	vector<QueryResult *> seeds;
	
	QueryVirtualResult();
	QueryVirtualResult(string _vo_name, LogicalID *_view_def, vector<QueryResult *> _seeds);
	QueryResult* clone();
	virtual ~QueryVirtualResult() { 
		for (unsigned int i = 0; i < (seeds.size()); i++ ) { delete (seeds.at(i)); };
		if (view_def != NULL) delete view_def;
		if (ec != NULL) delete ec; 
	};
	
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
	int nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
	
        virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
		string valueS = view_def ? view_def->toString() : "<null>";
		string res = getPrefixForLevel( level, n ) + "[Virtual] name=" + vo_name + " definition lid=" + valueS + "\n";
		for (unsigned int i = 0; i < (seeds.size()); i++ ) {
			res = res +  getPrefixForLevel( level + 1, n ) + "  [seed] " + seeds.at(i)->toString(level, recursive, n) + "\n";
		}
		return res;
        }
};


}

#endif  /* _QUERYRESULT_H_ */
