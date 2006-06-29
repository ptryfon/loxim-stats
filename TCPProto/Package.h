#ifndef _PACKAGE_H_
#define _PACKAGE_H_

#include "../Errors/Errors.h"
#include "../Driver/Result.h"
#include "../QueryExecutor/QueryResult.h"

namespace TCPProto {

using namespace Driver;
using namespace QExecutor;
using namespace std;
using namespace Errors;

class Package {
public:
    enum packageType {
	SIMPLEQUERY = 0,   //String
	PARAMQUERY = 1,	   //String with $var
	STATEMENT = 2,	   //Parser tree
	PARAMSTATEMENT = 3,//stmtNr + map
	SIMPLERESULT = 4   //Result
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
		Result* tmpRes;
		
		QueryResult* qr;
		ErrorConsole *ec;
		
		int dataDeserialize(Result** r);
		int grabElements(ResultCollection* col);
		int getULong(unsigned long &val);
		int stringCopy(char* &newBuffer);
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
	virtual ~ParamStatementPackage() {}
private:
	map<string, Result*> params;
	map<string, QueryResult*> queryParams;
	int ParamStatementPackage::getString(string* str);	
	int ParamStatementPackage::getQueryResult(QueryResult** result);	
	int ParamStatementPackage::setString(string str);	
	int ParamStatementPackage::setResult(Result* result);

	friend ostream& operator<<(ostream&, ParamStatementPackage&);	
};

} //namespace

#endif //_PACKAGE_H_
