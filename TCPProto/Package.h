#ifndef _PACKAGE_H_
#define _PACKAGE_H_

#include "../Driver/Result.h"

namespace TCPProto {

using namespace Driver;

class Package {
public:
    enum packageType {
	SIMPLEQUERY = 0, //String
	PARAMQUERY = 1,	//String with $var
	STATEMENT = 2,	//Parser tree
	SIMPLERESULT = 3 //Result
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

class StatementPackage : public Package {
public:
	packageType getType() {
		return SIMPLEQUERY;
	}
	
	int serialize(char** buffer, int* size) {
		return -1;
	}
	
	int deserialize(char* buffer, int size) {
		return -1;
	}
};

class SimpleQueryPackage : public Package {
public:
	packageType getType() {
		return SIMPLEQUERY;
	}
	
	int serialize(char** buffer, int* size) {
		return -1;
	}
	
	int deserialize(char* buffer, int size) {
		return -1;
	}

	void setQuery(const char* query) {
	}
};

class SimpleResultPackage : public Package {
public:
	packageType getType() {
		return SIMPLEQUERY;
	}
	
	int serialize(char** buffer, int* size) {
		return -1;
	}
	
	int deserialize(char* buffer, int size) {
		return -1;
	}

	Result * getResult() {
		return 0;
	}
};

class ParamQueryPackage : public Package {
public:
	packageType getType() {
		return SIMPLEQUERY;
	}
	
	int serialize(char** buffer, int* size) {
		return -1;
	}
	
	int deserialize(char* buffer, int size) {
		return -1;
	}

	void setQuery(const char* query) {
	}
};

} //namespace

#endif //_PACKAGE_H_
