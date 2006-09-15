#ifndef _PACKAGE_H_
#define _PACKAGE_H_

#include <iostream>
#include <string>

#include "../Errors/Errors.h"
#include "../Driver/Result.h"
#include "../QueryExecutor/QueryResult.h"
#include "TCPParam.h"

namespace TCPProto {

using namespace Driver;
using namespace QExecutor;
using namespace std;
using namespace Errors;

class Package {
public:
    enum packageType {
	RESERVED	= -1,	//to force nondefault deserialization method
	SIMPLEQUERY = 0,   //String
	PARAMQUERY = 1,	   //String with $var
	STATEMENT = 2,	   //Parser tree
	PARAMSTATEMENT = 3,//stmtNr + map
	SIMPLERESULT = 4,   //Result
	ERRORRESULT = 5, //Parse/execute error package
	REMOTEQUERY = 6, //remote reference
	REMOTERESULT = 7 //environment section
    };
    
	virtual packageType getType()=0;

    //returns error code, message buffer and size of the buffer
    //it doesn't destroy the buffer
    //first byte of the buffer is the resultType
    virtual int serialize(char** buffer, int* size)=0;
    
    //returns error code, gets buffer and it's size
    //it destroys the buffer
    virtual int deserialize(char* buffer, int size)=0;
};

class SimpleQueryPackage : public Package {

	public:
		packageType getType() { return SIMPLEQUERY; }
		int serialize(char** buffer, int* size);
		int deserialize(char* buffer, int size);
		void setQuery(const char* query);
		char* getQuery();
		int getQuerySize();

	private:
		const char* tmpBuffer;
		char* buf;
		int querySize;

};

class ErrorPackage : public Package {
	public:
	    packageType getType() {return ERRORRESULT;}
	    int serialize(char **buffer, int *size);
	    int deserialize(char *buffer, int size);
	    void setError(int errNo);
	    int getError();
	    Result* getResult();
	    void init();
	    void deinit();
	private:
	    Result* tmpRes;
	    int errorNo;
	    char *bufferB;
	    char *serialBuf;
	    ErrorConsole *ec;
};

class SimpleResultPackage : public Package {
	
	public:
		packageType getType() { return SIMPLERESULT; }
		int serialize(char** buffer, int *size);
		int deserialize(char* buffer, int size);
		Result * getResult();
		void setResult(Result *r);
		void setQueryResult(QueryResult *r);
		void prepareBuffers();
		void freeBuffers();	
		double htonDouble(double x);
		char *serialBuf;
		char *bufferBegin;
		int finalSize;
		char * bufferEnd;
		
	private:
		QueryResult* qr;
		ErrorConsole *ec;
		Result* tmpRes;
		int dataDeserialize(Result** r);
		int grabElements(ResultCollection* col);
		
	protected:
		int getULong(unsigned long &val);
		int stringCopy(char* &newBuffer);
		
};

class RemoteResultPackage : public Package {
	
	public:
		RemoteResultPackage(TCPParam* param);
		packageType getType() { return REMOTERESULT; }
		int serialize(char** buffer, int *size) {return -1;};
		int deserialize(char* buffer, int size);
		QueryResult* getQueryResult();
		virtual ~RemoteResultPackage(){}
		
	protected:
		int dataDeserialize(QueryResult** r);
		int toInt(int* result, string from);
		int grabElements(QueryResult* col);
		QueryResult* tmpRes;
		int getULong(unsigned long &val);
		int stringCopy(char* &newBuffer);
	
	private:
		string server;
		int port;
		ErrorConsole *ec;
		char *bufferBegin;
		char * bufferEnd;
};

class StatementPackage : public Package {
public:
	packageType    getType() { return STATEMENT; }
	int            serialize(char** buffer, int* size);
	int            deserialize(char* buffer, int size);
	void           setStmtNr(unsigned long stmtNr);
	unsigned long  getStmtNr();
	unsigned long  getSize();
protected:
	unsigned long stmtNr; //What nr of parsed statment. Server will match TreeNode with this nr
	char*         bufferBegin;
	char*         bufferEnd;

	int           getULong(unsigned long *val, char** buffer);
	int           getULong(unsigned long *val);
	int           setULong(unsigned long val,  char** buffer);
	int           setULong(unsigned long val);
};

class ParamQueryPackage : public SimpleQueryPackage {
public:
	packageType getType() { return PARAMQUERY; }
	int         serialize(char** buffer, int* size);
	int         deserialize(char* buffer, int size);
};

class ParamStatementPackage : public StatementPackage {
public:
	packageType getType() { return PARAMSTATEMENT; }
	int         serialize(char** buffer, int* size); 
	int         deserialize(char* buffer, int size);
	void        setParams(map<string, Result*> params) 
		{ ParamStatementPackage::params = params; }

	map<string, Result*> getParams() 
		{ return params; }

	void        setQueryParams(map<string, QueryResult*> queryParams) 
		{ ParamStatementPackage::queryParams = queryParams; }	  

	map<string, QueryResult*> getQueryParams() 
		{ return queryParams; }

	string      toString();

	virtual ~ParamStatementPackage() {}
private:
	map<string, Result*> params;
	map<string, QueryResult*> queryParams;
	int getString(string* str);	
	int setString(string str);
	int getStringLength(string str);	
	int setDouble(double val);
	int getDouble(double *val);
	int setResult(Result* result);
	int setResultCollection(ResultCollection* result);
	int setResultOld(Result* result);
	int getResultCollectionLength(ResultCollection* collection);
	int getResultLength(Result* result);
	int getQueryResultCollection(QueryResult* collection);
	int getQueryResult(QueryResult** result);		

	friend ostream& operator<<(ostream&, ParamStatementPackage&);	
};

class RemoteQueryPackage : public Package {

	public:
		packageType getType() { return REMOTEQUERY; }
		int serialize(char** buffer, int* size); 
		int deserialize(char* buffer, int size);
		void setLogicalID(LogicalID* lid) {this->lid = lid;}
		LogicalID* getLogicalID() {return lid;}
		RemoteQueryPackage(){lid = NULL;}
		virtual ~RemoteQueryPackage(){}
		bool isDeref() {return deref;}
		void setDeref(bool d) {deref = d;}
		
	protected:
		int stringCopy(char* &newBuffer);
		LogicalID* lid;
		bool deref;
		char * bufferEnd;
		char *bufferBegin;
};

} //namespace

#endif //_PACKAGE_H_
