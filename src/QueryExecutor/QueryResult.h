#ifndef _QUERYRESULT_H_
#define _QUERYRESULT_H_

#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>

#include <QueryExecutor/InterfaceKey.h>
#include <QueryParser/ClassNames.h>
#include <TransactionManager/Transaction.h>
#include <Store/Store.h>
#include <Store/DBDataValue.h>
#include <Store/DBLogicalID.h>
#include <QueryParser/QueryParser.h>
#include <QueryParser/TreeNode.h>
#include <Errors/Errors.h>
#include <Errors/ErrorConsole.h>

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

template<typename T> struct ptr_less {
	bool operator () (T *a, T *b) {
		return a->less_than(b);
	}
};

template<typename T> struct ptr_less_in_orderBy {
	bool operator () (T *a, T *b) {
		return a->sorting_strict_less(b);
	}
};

class QueryResult : public Countable<QueryResult>
{
protected:
	ErrorConsole *ec;
public:
	enum QueryResultType {
		QRESULT   = 0,
		QREFERENCE= 1,
		QVIRTUAL  = 2,
		QSTRING   = 3,
		QINT      = 4,
		QDOUBLE   = 5,
		QBOOL     = 6,
		QBINDER   = 7,
		QSTRUCT   = 8,
		QBAG      = 9,
		QSEQUENCE =10,
		QNOTHING  =11
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
	bool not_equal(QueryResult *r);
	bool greater_than(QueryResult *r);
	virtual bool less_than(QueryResult *r)=0;
	bool greater_eq(QueryResult *r);
	bool less_eq(QueryResult *r);
	int nested_and_push_on_envs(QueryExecutor * qe, Transaction *&tr);
	virtual int nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec)=0;
	virtual bool isBool()=0;
	virtual bool isNothing()=0;
	virtual int getBoolValue(bool &b)=0;
	virtual bool isReferenceValue()=0;
	virtual int getReferenceValue(QueryResult *&r)=0;
	virtual bool sorting_strict_less(QueryResult *arg);
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
	int nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
	void sortSequence();
	
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
	int nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
	void sortBag();
	void sortBag_in_orderBy();
	int postSort(QueryResult *&f);
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
	int nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
	void sortStruct();
	
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
	int nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec);
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
	int nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec);
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
	int nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec);
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
	int nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec);
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
	int nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec);
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
	InterfaceKey m_key;
public:
	QueryReferenceResult();
	QueryReferenceResult(LogicalID* v);
	QueryReferenceResult(LogicalID* v, InterfaceKey k);
	QueryResult* clone();
	virtual ~QueryReferenceResult() { if (value != NULL) delete value; if (ec != NULL) delete ec; };
	LogicalID* getValue();
	void setValue(LogicalID* v);
	InterfaceKey getInterfaceKey() const;
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
	int nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec);
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
	int nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec);
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
	vector<LogicalID *> view_defs;
	vector<QueryResult *> seeds;
	bool refed;
	LogicalID *view_parent;
	
	QueryVirtualResult();
	QueryVirtualResult(string _vo_name, vector<LogicalID *> _view_defs, vector<QueryResult *> _seeds, LogicalID *_view_parent);
	QueryResult* clone();
	virtual ~QueryVirtualResult() { 
		seeds.clear();
		view_defs.clear();
		if (ec != NULL) delete ec;
		if (view_parent != NULL) delete view_parent; 
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
	int nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec);
	bool isBool();
	bool isNothing();
	int getBoolValue(bool &b);
	bool isReferenceValue();
	int getReferenceValue(QueryResult *&r);
	
        virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
		string valueS = view_defs.at(0) ? view_defs.at(0)->toString() : "<null>";
		string res = getPrefixForLevel( level, n ) + "[Virtual] name=" + vo_name + " definition lid=" + valueS + "\n";
		for (unsigned int i = 0; i < (seeds.size()); i++ ) {
			res = res +  getPrefixForLevel( level + 1, n ) + "  [seed] " + seeds.at(i)->toString(level, recursive, n) + "\n";
		}
		return res;
        }
};


}

#endif  /* _QUERYRESULT_H_ */
