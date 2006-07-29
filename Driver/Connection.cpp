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

#include "../TCPProto/Tcp.h"
#include "Connection.h"
#include "Result.h"
#include "../TCPProto/Package.h"

namespace Driver {

using namespace std;
using namespace TCPProto;


Connection::Connection(int socket)
{
	sock = socket;
}

Connection::~Connection()
{
}

int Connection::stringCopy(char* &newBuffer) { // od bufferBegin
		int len = strlen(bufferBegin)+1; //including NULL

		if (bufferBegin + len > bufferEnd) {
			cerr << "<Serialize::StringCopy> proba czytania poza buforem" << endl;
			throw ConnectionProtocolException();
		}

		newBuffer = (char*)malloc (len);

		if (NULL == newBuffer) {
			throw ConnectionMemoryException(errno | ErrDriver);
		}
		strcpy (newBuffer, bufferBegin); // always succeeds
		bufferBegin += len;
		return 0;
}

unsigned long Connection::getULong(unsigned long &val) {

	char* tmpPtr = bufferBegin + sizeof(long);

	if (tmpPtr > bufferEnd) {
		cerr << "<Connetion::getULong> proba czytania poza odebranym buforem" << endl;
		throw ConnectionProtocolException();
	}

		val = ntohl(*((unsigned long*) bufferBegin));
		bufferBegin = tmpPtr; //skip the number of elements (long)
		return val;
}


Result* Connection::grabElements(ResultCollection* col) {

			auto_ptr<Result> resultPtr(col); //it's the root of the subtree

			unsigned long number;
			getULong(number);
//			col->setSize(getULong(number)); //by reference
			for (unsigned long i = 1; i <= number; i++) {
				col->add(deserialize());
			}

			resultPtr.release(); // the subtree has been built, don't destroy it.
			//if anything fails it will be destroyed by the parent
			return col;
}


Result* Connection::deserialize() {
//	int error;
//	ResultBag *brs;
//	Result* result;
	char* id;
	unsigned long number;
//	unsigned long i;
	unsigned long* lptr;
	double db;
	char df;

	switch (*(bufferBegin++)) {
		case Result::BAG:
			if (print_err) cerr << "<Connection::deserialize> tworze obiekt BAG\n";
			return grabElements(new ResultBag());

		case Result::SEQUENCE:
			if (print_err) cerr << "<Connection::deserialize> tworze obiekt SEQUENCE\n";
			return grabElements(new ResultSequence());

		case Result::STRUCT:
			if (print_err) cerr << "<Connection::deserialize> tworze obiekt STRUCT\n";
			return grabElements(new ResultStruct());


		case Result::REFERENCE:
			if (print_err) cerr << "<Connection::deserialize> tworze obiekt REFERENCE\n";
			stringCopy(id); //by reference
			return new ResultReference(string (id));

		case Result::VOID:
			if (print_err) cerr << "<Connection::deserialize> tworze obiekt VOID\n";
			return new ResultVoid();

	        case Result::STRING:
			if (print_err) cerr << "<Connection::deserialize> tworze obiekt STRING\n " ;
			stringCopy(id); // by reference
			return new ResultString(string (id));

		case Result::ERROR:
			if (print_err) cerr << "<Connection::deserialize> tworze obiekt ERROR\n";
			getULong(number); //by reference
			throw ConnectionServerException((int)number);
			//return new ResultError(number);


		case Result::INT:
			if (print_err) cerr << "<Connection::deserialize> tworze obiekt INT\n";
			getULong(number); //by reference
			return new ResultInt((int) number);

		case Result::BOOLTRUE:
			if (print_err) cerr << "<Connection::deserialize> tworze obiekt BOOL (true)\n";
			return new ResultBool(true);

		case Result::BOOLFALSE:
			if (print_err) cerr << "<Connection::deserialize> tworze obiekt BOOL (false)\n";
			return new ResultBool(false);

		case Result::DOUBLE:
			if (print_err) cerr << "<Connection::deserialize> tworze obiekt DOUBLE\n";
			lptr = (unsigned long*) &db;
			getULong(*lptr); //higher word
			getULong(*(lptr+1)); //lower word
			return new ResultDouble(db);

		case Result::BINDER:
			if (print_err) cerr << "<Connection::deserialize> tworze obiekt BINDER\n";
			stringCopy(id); // by reference
			//TODO stringi robic jako new i puszczac jako referencja - wtedy trzeba miec pewnosc ze jak konstruktor sie posypie to zostana usuniete
			return new ResultBinder(string(id), deserialize());

		default:
			df = *(bufferBegin-1);
			if (print_err) cerr << "<Connection::deserialize> obiekt nieznany, nr: " << (int) df << endl;
			throw ConnectionProtocolException();
	} // switch
} // deserialize

Result* Connection::oldReceive() throw (ConnectionException) {
	int error;
	char* ptr = NULL;
      int ile;
      error = bufferReceive(&ptr, &ile, sock); //create buffer and set ptr to point at it

      if (error != 0) { //ptr was free
     	throw ConnectionIOException(error);
      }
      if (ile == 0) { //no error but also no data -> server was closed
      	throw ConnectionClosedException();
      }
      //bufferHandler will free memory pointed by ptr at the end of a scope
      BufferHandler bufferPtr(ptr); //needed only during deserializing (exception possible)
      bufferBegin = ptr;
      bufferEnd = ptr+ile;

      Result* rs = deserialize();

//      cerr << "<Connection::execute> obiekt Result stworzony -> procedura zakonczona sukcesem" << endl;
      return rs;
}

Result* Connection::oldExecute(const char* query) throw (ConnectionException) {

	int error;
      error = bufferSend(query, strlen(query)+1, sock);
      if (0 != error) {
      	throw ConnectionIOException(error);
      }
      char* ptr = NULL;
      int ile;
      error = bufferReceive(&ptr, &ile, sock); //create buffer and set ptr to point at it

      if (error != 0) { //ptr was free
     	throw ConnectionIOException(error);
      }
      if (ile == 0) { //no error but also no data -> server was closed
      	throw ConnectionClosedException();
      }
      //bufferHandler will free memory pointed by ptr at the end of a scope
      BufferHandler bufferPtr(ptr); //needed only during deserializing (exception possible)
//      cerr << "<Connection::execute> driver przyszlo bajtow: " << ile << endl;
      bufferBegin = ptr;
      bufferEnd = ptr+ile;

      Result* rs = deserialize();

//      cerr << "<Connection::execute> obiekt Result stworzony -> procedura zakonczona sukcesem" << endl;
      return rs;
}

Result* Connection::execute(const char* query) throw (ConnectionException) {
	SimpleQueryPackage spackage;
	//cerr << "zaczynam execute\n";
	spackage.setQuery(query);
	int error;
    error = packageSend(&spackage, sock);
    //cerr << "kod bledu wysylania: "<< error << endl;
    //TODO lepsza diagnostyka bledow
	if (error != 0) throw ConnectionIOException(error);
	Package* package;
	return oldReceive();
    error = packageReceive(&package, sock);
    //cerr << "kod bledu odbioru: "<< error << endl;
	if (error != 0) throw ConnectionIOException(error);

	if (package->getType() != Package::SIMPLERESULT) throw ConnectionException("incorrect data received");
	 SimpleResultPackage* srp = dynamic_cast<SimpleResultPackage*> (package);
	return srp->getResult();
}

void Connection::set_print_err(bool _print_err) {
    this->print_err = _print_err;
}
Result* Connection::execute(Statement* stmt) throw (ConnectionException) {
	int error;

	ParamStatementPackage* psp = new ParamStatementPackage();
	psp->setStmtNr(stmt->stmtNr);
	psp->setParams(stmt->params);
	
	error = packageSend(psp, sock);
	if (error != 0) throw ConnectionIOException(error);
	delete psp;

	Package* package;
	error = packageReceive(&package, sock);
	if (error != 0) throw ConnectionIOException(error);
	
	if (package->getType() != Package::SIMPLERESULT) throw ConnectionException("incorrect data received");
	SimpleResultPackage* srp = dynamic_cast<SimpleResultPackage*> (package);
	return srp->getResult();
}


Statement* Connection::parse(const char* query) throw (ConnectionException) {
	int error;
	ParamQueryPackage t;
	t.setQuery(query);
	error = packageSend(&t, sock);
	if (error != 0) throw ConnectionIOException(error);

	Package* package;
	error = packageReceive(&package, sock);
	if (error != 0) throw ConnectionIOException(error);

	if (package->getType() != Package::STATEMENT) throw ConnectionException("incorrect data received");
	StatementPackage* stmtPkg = dynamic_cast<StatementPackage*> (package);
	
	return new Statement(stmtPkg->getStmtNr());
}


int Connection::disconnect() {
	return close(sock);
}


ostream& operator<<(ostream& os, ConnectionException& e) {
	e.toStream(os);
	return os;
}



} // namespace
