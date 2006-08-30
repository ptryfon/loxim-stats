#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <string.h>
#include <memory.h>
#include <errno.h>
#include "../Errors/Errors.h"
#include "unistd.h"

//#include "Tcp.h"
//#include "../Driver/Connection.h"
#include "../Driver/Result.h"
//#include "../QueryExecutor/QueryResult.h"
#include "Package.h"

namespace TCPProto {

using namespace std;
using namespace QExecutor;
//using namespace TCPProto;
	
	
	void SimpleResultPackage::setQueryResult(QueryResult *qRes) {
	    qr = qRes;
	}

	void SimpleResultPackage::prepareBuffers() {
	    int mesSize = 8192;
	    bufferBegin = (char *)malloc(mesSize);
	    bufferEnd = bufferBegin + mesSize;
	    memset(bufferBegin, '\0', mesSize);
	    serialBuf = bufferBegin;
		finalSize = 0;
		ec = new ErrorConsole("SimpleResultPackage");
	    
		serialBuf[0] = (char)Package::SIMPLERESULT;
		serialBuf++;
		finalSize = finalSize + 1;

	}
	
	void SimpleResultPackage::freeBuffers() {
	    free(bufferBegin);
	}

	double SimpleResultPackage::htonDouble(double inD) {
		double res;
		unsigned int *in = (unsigned int *)(&inD);
		unsigned int *out = (unsigned int *)(&res);
		out[0]=htonl(in[0]);
		out[1]=htonl(in[1]);
		return res;
	}
	
	int SimpleResultPackage::serialize(char** buffer, int *size) {
	
	    QueryResult *qres;
	    QueryResult *collItem;
	    QueryStringResult *stringRes;
	    QueryBagResult *bagRes;
	    QueryReferenceResult *refRes;
	    QueryIntResult *intRes;
	    QueryBoolResult *boolRes;
    	    QueryDoubleResult *doubleRes;
	    QueryBinderResult *bindRes;
	

	    int rT;
	    Result::ResultType resType;
	
	    int valSize;
	    int bagSize;
	    unsigned long bufBagLen;
	    unsigned int intVal;
	    double doubleVal;
	    bool boolVal;
	    string strVal;
	    int retVal;
	    int i;

	    resType=(Result::ResultType)qr->type();
	    rT=qr->type();
		
	    ec->printf("[SimpleResultPackage.Serialize]--> Now serializing object of type: %d \n",  rT);

	switch (rT) {
		case QueryResult::QBAG:
			resType=Result::BAG;
			bagSize=qr->size();
			bufBagLen=(unsigned long) bagSize;
			ec->printf("[SimpleResultPackage.Serialize]--> Received BAG of size %lu \n", bufBagLen);
			bufBagLen=htonl(bufBagLen);
			if (qr->collection() != true) {
				*ec << "[SimpleResultPackage.Serialize]-->QueryResult shows type of BAG and says it is no collection";
				return ErrServer+EBadResult;
				return -1;
			}
			bagRes = (QueryBagResult *)qr;
			serialBuf[0]=(char)resType;
			serialBuf++;
			finalSize = finalSize + 2;
			memcpy((void *)serialBuf, (const void *)&bufBagLen, sizeof(bufBagLen));
			//ec->printf("[SimpleResultPackage.Serialize]--> Bag header: |(type)=%d|%d|(size)=%lu|\n", 
			//(int)bufferBegin[0], (int)bufferBegin[1], ntohl(*(unsigned long *)serialBuf));
			serialBuf=serialBuf+sizeof(bufBagLen);
			finalSize = finalSize + sizeof(bufBagLen);

			//TODO DEPTH
			for (i=0;i<bagSize;i++) {
				ec->printf("[SimpleResultPackage.Serialize]--> Serializing collection item %d \n", i);
				retVal=bagRes->getResult(collItem);
				if (retVal!=0) {
				    ec->printf("[SimpleResultPackage.Serialize]--> getResult failed with %d, returning.. \n", retVal);
				    return retVal;
				}
				qr = (QueryResult *)collItem;
				serialize(buffer, size);
			}
			break;
		case QueryResult::QSEQUENCE:
			resType=Result::SEQUENCE;
			bagSize=qr->size();
			bufBagLen=(unsigned long) bagSize;
			ec->printf("[SimpleResultPackage.Serialize]--> Received SEQUENCE of size %lu \n", bufBagLen);
			bufBagLen=htonl(bufBagLen);
			if (qr->collection() != true) {
				*ec << "[SimpleResultPackage.Serialize]-->QueryResult shows type of SEQUENCE and says it is no collection";
				return ErrServer+EBadResult;
				return -1;
			}
			bagRes = (QueryBagResult *)qr;
			*ec << "[SimpleResultPackage.Serialize]--> Adding sequence header";
			serialBuf[0]=(char)resType;
			serialBuf++;
			finalSize = finalSize + 2;
			memcpy((void *)serialBuf, (const void *)&bufBagLen, sizeof(bufBagLen));
			serialBuf=serialBuf+sizeof(bufBagLen);
			finalSize = finalSize + sizeof(bufBagLen);

			//TODO DEPTH
			for (i=0;i<bagSize;i++) {
				ec->printf("[SimpleResultPackage.Serialize]--> Serializing collection item %d \n", i);
				retVal=bagRes->getResult(collItem);
				if (retVal!=0) {
				    ec->printf("[SimpleResultPackage.Serialize]--> getResult failed with %d, returning.. \n", retVal);
				    return retVal;
				}
				qr = (QueryResult *)collItem;
				serialize(buffer, size);
			}

			break;
		case QueryResult::QSTRUCT:
			resType=Result::STRUCT;
			bagSize=qr->size();
			bufBagLen=(unsigned long) bagSize;
			ec->printf("[SimpleResultPackage.Serialize]--> Received STRUCT of size %lu \n", bufBagLen);
			bufBagLen=htonl(bufBagLen);
			if (qr->collection() != true) {
				*ec << "[SimpleResultPackage.Serialize]-->QueryResult shows type of STRUCT and says it is no collection";
				return ErrServer+EBadResult;
			}
			bagRes = (QueryBagResult *)qr;
			//*ec << "[SimpleResultPackage.Serialize]--> Adding struct header";
			serialBuf[0]=(char)resType;
			serialBuf++;
			finalSize = finalSize + 2;
			memcpy((void *)serialBuf, (const void *)&bufBagLen, sizeof(bufBagLen));
			serialBuf=serialBuf+sizeof(bufBagLen);

			//TODO DEPTH
			for (i=0;i<bagSize;i++) {
				ec->printf("[SimpleResultPackage.Serialize]--> Serializing collection item %d \n", i);
				retVal=bagRes->getResult(collItem);
				if (retVal!=0) {
				    ec->printf("[SimpleResultPackage.Serialize]--> getResult failed with %d, returning.. \n", retVal);
				    return retVal;
				}
				qr = (QueryResult *)collItem;
				serialize(buffer, size);
			}

			break;
		case QueryResult::QREFERENCE:
		    *ec << "[SimpleResultPackage.Serialize]--> Getting reference";
		    resType=Result::REFERENCE;
		    serialBuf[0]=(char)resType;
		    refRes = (QueryReferenceResult *)qr;
		    strVal=(refRes->getValue())->toString();
		    serialBuf++;
		    valSize=strVal.length();
		    ec->printf("[SimpleResultPackage.Serialize]--> Reference value: strVal=%s \n", strVal.c_str());
		    strcpy(serialBuf, strVal.c_str());
		    ec->printf("[SimpleResultPackage.Serialize]--> Reference string serialized to: %s \n", serialBuf);
		    serialBuf=serialBuf+valSize+1;
		    break;
		case QueryResult::QSTRING:
		    *ec << "[SimpleResultPackage.Serialize]--> Getting STRING";
		    resType=Result::STRING;
		    stringRes=(QueryStringResult *)qr;
		    strVal=stringRes->getValue();
		    valSize=strVal.length();
		    ec->printf("[SimpleResultPackage.Serialize]--> Adding string header, string size=%d \n", valSize);
		    serialBuf[0]=(char)resType;
		    serialBuf++;
		    strcpy(serialBuf, strVal.c_str());
		    ec->printf("[SimpleResultPackage.Serialize]--> String serialized to: %s \n", serialBuf);
		    serialBuf=serialBuf+valSize+1;
		    break;
		case QueryResult::QRESULT:
		    *ec << "[SimpleResultPackage.Serialize]--> Getting RESULT (ERROR!)";
		    return -1; //TODO ERRORCODE
		    break;
		 case QueryResult::QNOTHING:
		    *ec << "[SimpleResultPackage.Serialize]--> VOID type, sending type indicator only";
		    resType=Result::VOID;
		    serialBuf[0]=(char)resType;
		    *ec << "[SimpleResultPackage.Serialize]--> VOID type written";
		    serialBuf++;
		    break;
		case QueryResult::QINT:
		    *ec << "[SimpleResultPackage.Serialize]--> INT type, sending type indicator..";
		    resType=Result::INT;
		    serialBuf[0]=(char)resType;
		    serialBuf++;
		    intRes = (QueryIntResult *)qr;
		    intVal = intRes->getValue();
		    ec->printf("[SimpleResultPackage.Serialize]--> .. followed by int value (%d) \n", intVal);
		    intVal=htonl(intVal);
		    memcpy((void *)serialBuf, (const void *)&intVal, sizeof(intVal));
		    serialBuf=serialBuf+sizeof(intVal);
		    *ec << "[SimpleResultPackage.Serialize]--> buffer written";
		    break;
		case QueryResult::QDOUBLE:
		    *ec << "[SimpleResultPackage.Serialize]--> DOUBLE type";
		    resType=Result::DOUBLE;
		    serialBuf[0]=(char)resType;
		    serialBuf++;
		    doubleRes = (QueryDoubleResult *)qr;
		    doubleVal = doubleRes->getValue();
		    ec->printf("[SimpleResultPackage.Serialize]--> double value is %lf \n", doubleVal);
		    *ec << "[SimpleResultPackage.Serialize]--> Switching double byte order..";
		    doubleVal = htonDouble(doubleVal);
		    *ec << "[SimpleResultPackage.Serialize]--> Splitting and byte order change complete";
		    memcpy((void *)serialBuf, (const void *)&doubleVal, sizeof(doubleVal));
		    serialBuf = serialBuf + sizeof(doubleVal);
		    break;
		case QueryResult::QBOOL:
		    *ec << "[SimpleResultPackage.Serialize]--> BOOL type, sending type indicator and value";
		    boolRes=(QueryBoolResult *)qr;
		    boolVal=boolRes->getValue();
		    if (boolVal==false)
		        resType=Result::BOOLFALSE;
		    else if (boolVal==true)
			resType=Result::BOOLTRUE;
		    else {
			ec->printf("[SimpleResultPackage.Serialize]-->Bool result from Executor corrupted: |%d| \n", (int)boolVal);
			*ec << "[SimpleResultPackage.Serialize]-->Sending FALSE..";
			resType=Result::BOOLFALSE;
		    }
		    serialBuf[0]=(char)resType;
		    boolRes = (QueryBoolResult *)qr;
		    serialBuf++;
		    serialBuf[0]=(char)(boolVal);
		    serialBuf++;
		    ec->printf("[SimpleResultPackage.Serialize]--> Sending bool: |type=%d|value=%d|\n", (int)resType, (int)boolVal);
		    break;
		case QueryResult::QBINDER:
		    *ec << "[SimpleResultPackage.Serialize]--> BINDER type, sending name(string) and result";
		    resType=Result::BINDER;
		    serialBuf[0]=(char)resType;
		    serialBuf++;
		    bindRes=(QueryBinderResult *) qr;
		    strVal=bindRes->getName();
		    qres=bindRes->getItem();
		    strcpy(serialBuf, strVal.c_str());
		    serialBuf=serialBuf+(strVal.length())+1;
		    //SerializeRec(qres);
		    qr = (QueryResult *)qres;
		    serialize(buffer, size);
		    break;
		default:
		    *ec << "[SimpleResultPackage.Serialize]--> object UNRECOGNIZED !";
		    return -1; //TODO ERROR
		    break;
	}
	
	*buffer = &*bufferBegin;
	*size = 8192; //&finalSize;
	//ec->printf("Buffer = %s\n", buffer);
	//ec->printf("Buffer set with type = %d|%d|%d|%d|\n", (int)(*buffer[0]), (int)(*buffer[1]), (int)(*buffer[2]), (int)(*buffer[3]));

	return 0;
	
	}
	
	void SimpleResultPackage::setResult(Result *r) {
		tmpRes = r;
	}
	
	Result * SimpleResultPackage::getResult() {
		return tmpRes;
	}

	int SimpleResultPackage::stringCopy(char* &newBuffer) { // od bufferBegin
		int len = strlen(bufferBegin)+1; //including NULL
		
		if (bufferBegin + len > bufferEnd) {
			return -2; //TODO error czytanie poza buforem
		}
		
		newBuffer = (char*)malloc (len);
		
		if (NULL == newBuffer) {
			return (errno | ErrDriver);
		}
		strcpy (newBuffer, bufferBegin); // always succeeds
		bufferBegin += len;
		return 0;
}

int SimpleResultPackage::getULong(unsigned long &val) {
	
	char* tmpPtr = bufferBegin + sizeof(long);
	
	if (tmpPtr > bufferEnd) {
		return -2; //TODO proba czytania poza buforem
	}
	
		val = ntohl(*((unsigned long*) bufferBegin));
		bufferBegin = tmpPtr; //skip the number of elements (long)
		return 0;
}


int SimpleResultPackage::grabElements(ResultCollection* col) {
			
			unsigned long number;
			int error;
			if (0 != (error = getULong(number))) return error;
			Result* r;
//			col->setSize(getULong(number)); //by reference
			for (unsigned long i = 1; i <= number; i++) {
				if (0 != (error = dataDeserialize(&r))) return error;
				col->add(r);
			}
			
			return 0;
}

 int SimpleResultPackage::deserialize(char* buffer, int size) {
 	if (buffer[0] != getType()) return -1; //TODO
	bufferBegin = buffer+1;
	bufferEnd = buffer+size;
	int error;
	Result* r;
	error = dataDeserialize(&r);
	if (error != 0) {
		if (r != NULL) delete r; //a part of tree was created
		return error;
	}
	tmpRes = r;
	free (buffer);
	return 0;
 }
  
int SimpleResultPackage::dataDeserialize(Result** r ) {
//	int error;
//	ResultBag *brs;
//	Result* result;	
	char* id;
	int error;
	unsigned long number;
//	unsigned long i;
	unsigned long* lptr;
	double db;
	char df;
	//printf("dataDeserialize: buffer[0], buffer[1], buffer[2], buffer[3], buffer[4] = |%d|%d|%d|%d|%d|\n",(int)bufferBegin[0], (int)bufferBegin[1], (int)bufferBegin[2], (int)bufferBegin[3], (int)bufferBegin[4]);	
	
	switch (*(bufferBegin++)) {
	
		case Result::BAG:
			cerr << "<Connection::deserialize> tworze obiekt BAG\n";
			*r = new ResultBag();
			return grabElements((ResultCollection*) *r);
			
		case Result::SEQUENCE:
			cerr << "<Connection::deserialize> tworze obiekt SEQUENCE\n";
			*r = new ResultSequence();
			return grabElements((ResultCollection*) *r);
		
		case Result::STRUCT:
			cerr << "<Connection::deserialize> tworze obiekt STRUCT\n";
			*r = new ResultStruct();
			return grabElements((ResultCollection*) *r);
			
		case Result::REFERENCE:
			cerr << "<Connection::deserialize> tworze obiekt REFERENCE\n";
			error = stringCopy(id); //by reference
			if (error != 0) return error;
			*r = new ResultReference(string (id));
			return 0;
		
		case Result::VOID:
		cerr << "<Connection::deserialize> tworze obiekt VOID\n";
			*r = new ResultVoid();
			return 0;
		
	        case Result::STRING:
		  cerr << "<Connection::deserialize> tworze obiekt STRING\n " ;
			error = stringCopy(id); // by reference
			if (error != 0) return error;
			*r = new ResultString(string (id));
			return 0;
		
		case Result::ERROR:
			cerr << "<Connection::deserialize> tworze obiekt ERROR\n";
			error = getULong(number); //by reference
			if (error != 0) return error;
			*r = new ResultError(number);
			return 0;
			
		case Result::INT:
			cerr << "<Connection::deserialize> tworze obiekt INT\n";
			error = getULong(number); //by reference
			if (error != 0) return error;
			*r = new ResultInt((int) number);
			return 0;
		
		case Result::BOOLTRUE:
			cerr << "<Connection::deserialize> tworze obiekt BOOL (true)\n";
			*r = new ResultBool(true);
			return 0;
		
		case Result::BOOLFALSE:
			cerr << "<Connection::deserialize> tworze obiekt BOOL (false)\n";
			*r = new ResultBool(false);
			return 0;
		
		case Result::DOUBLE:
			cerr << "<Connection::deserialize> tworze obiekt DOUBLE\n";
			lptr = (unsigned long*) &db;
			error = getULong(*lptr); //higher word
			if (error != 0) return error;
			error = getULong(*(lptr+1)); //lower word
			if (error != 0) return error;
			*r = new ResultDouble(db);
			return 0;
		
		case Result::BINDER:
			cerr << "<Connection::deserialize> tworze obiekt BINDER\n";
			error = stringCopy(id); // by reference
			if (error != 0) return error;
			Result* subRes;
			error = dataDeserialize(&subRes);
			if (error != 0) return error;
			*r = new ResultBinder(string(id), subRes);
			return 0;
			
		default:
			df = *(bufferBegin-1);
			cerr << "<Connection::deserialize> obiekt nieznany, nr: " << (int) df << endl;
			return -1; //TODO
	} // switch
} // deserialize

};
