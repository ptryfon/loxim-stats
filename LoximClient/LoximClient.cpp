#include "LoximClient.h"

#include <string>
#include <stdlib.h>
#include <iostream>
#include <protocol/sockets/TCPIPClientSocket.h>
#include <protocol/layers/ProtocolLayer0.h>
#include <protocol/packages/VSCSendValuesPackage.h>
#include <protocol/packages/VSCSendValuePackage.h>
#include <protocol/packages/VSCFinishedPackage.h>
#include <protocol/packages/WCHelloPackage.h>
#include <protocol/packages/WSHelloPackage.h>
#include <protocol/packages/WCLoginPackage.h>
#include <protocol/packages/WSAuthorizedPackage.h>
#include <protocol/packages/WCPasswordPackage.h>
#include <protocol/packages/ASCOkPackage.h>
#include <protocol/packages/ASCByePackage.h>
#include <protocol/packages/QCStatementPackage.h>
#include <protocol/packages/data/BagData.h>
#include <protocol/packages/data/SequenceData.h>
#include <protocol/packages/data/ReferenceData.h>
#include <protocol/packages/data/VarcharData.h>
#include <protocol/packages/data/VOIDData.h>
#include <protocol/packages/data/Sint32Data.h>
#include <protocol/packages/data/DoubleData.h>
#include <protocol/packages/data/BoolData.h>
#include <protocol/packages/data/BindingData.h>
#include <protocol/packages/data/DataPart.h>
#include <protocol/packages/data/StructData.h>
#include <protocol/ptools/CharArray.h>
#include <Errors/Errors.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "Authenticator.h"
#include <readline/readline.h>
#include <signal.h>

#include <protocol/auth/auth.h>


#define CLIENT_NAME 	"LoximClient"
#define CLIENT_VERSION "1.0"

using namespace protocol;
using namespace std;

LoximClient::LoximClient::LoximClient(const char* a_hostname, int a_port)
{
	hostname=strdup(a_hostname);
	port=a_port;
	socket=NULL;
	proto=NULL;
	ws_hello=NULL;
}

LoximClient::LoximClient::~LoximClient()
{
	if (hostname)
		free(hostname);
	
	if (socket)
		delete socket;
		
	if (proto)
		delete proto;
}

int LoximClient::LoximClient::connect()
{
	if (socket)
		return -1;
	
	socket=new TCPIPClientSocket(hostname,port);
	int res=(((TCPIPClientSocket*)socket)->connect());
	
	if (res>0)
	{
		proto=new ProtocolLayer0(socket);
	};
	
	return res;
}

		

int LoximClient::LoximClient::run(StatementProvider *provider)
{
	string stmt;
	waiting_for_result = false;
	pthread_mutex_init(&logic_mutex, 0);
	pthread_cond_init(&read_cond, 0);
	pthread_t result_handler;
	error = 0;
	pthread_create(&result_handler, 0, ::LoximClient::result_handler_starter, this);
	pthread_mutex_t cond_mutex;
	pthread_mutex_init(&cond_mutex, 0);

	while (true){
		stmt = provider->read_stmt();
		pthread_mutex_lock(&logic_mutex);
		if (!strcmp(stmt.c_str(), "quit")){
			pthread_cancel(result_handler);
			pthread_mutex_unlock(&logic_mutex);
			ASCByePackage package("");
			if (!proto->writePackage(&package))
				return 1;
			return 0;
		}
		QCStatementPackage package(STATEMENT_FLAG_EXECUTE, const_cast<char*>(stmt.c_str())); 
		if (!proto->writePackage(&package)){
			pthread_cancel(result_handler);
			pthread_mutex_unlock(&logic_mutex);
			return error;
		}
		else{
			waiting_for_result = true;
			pthread_cond_wait(&read_cond, &logic_mutex);
			pthread_mutex_unlock(&logic_mutex);
			if (error)
				return error;
		}	
	}
}


void *::LoximClient::result_handler_starter(void *a)
{
	((LoximClient::LoximClient*)a)->result_handler();
	return NULL;
}

void LoximClient::LoximClient::print_error(ASCErrorPackage *package)
{
	printf("Got error %d: %s\n", package->getError_code(), package->getReason()->getBuffor());
}


void LoximClient::LoximClient::result_handler()
{
	VSCSendValuePackage *pack;
	ASCErrorPackage *err_pack;
	while(true){
		int res = receive_result(&pack, &err_pack);
		//if error in protocol occurred or the server wants to close the connection
		if (res > 0 || res == -1){
			error = res;
			pthread_cond_signal(&read_cond);
			raise(SIGTERM);
			pthread_exit(0);
		} else {
			pthread_mutex_lock(&logic_mutex);
			if (waiting_for_result){
				if (res == -2){
					print_error(err_pack);
					delete err_pack;
				} else {
					print_result(pack->getData(), 4);
					delete pack;
				}
				error = 0;
				waiting_for_result = false;
			} else {
				if (res == -2)
					delete err_pack;
				else
					delete pack;
				error = EProtocol;
				pthread_cond_signal(&read_cond);
				pthread_mutex_unlock(&logic_mutex);
				pthread_exit(0);
			}
			pthread_cond_signal(&read_cond);
			pthread_mutex_unlock(&logic_mutex);
		}
	}
}

//0 - successfuly received a result or an error report
//-1 - received bye
//-2 - received error report
//>0 - error
int LoximClient::LoximClient::receive_result(VSCSendValuePackage **res, ASCErrorPackage **err_res)
{
	Package *p, *p2;
	p = proto->readPackage();
	if (!p)
		return EReceive;
	if (p->getPackageType() == ID_ASCErrorPackage){
		*err_res = (ASCErrorPackage*)p;
		return -2;
	}
	if (p->getPackageType() == ID_ASCByePackage){
		delete p;
		return -1;
	}
	if (p->getPackageType() != ID_VSCSendValuesPackage){
		delete p;
		return EProtocol;
	}
	delete p;
	
	p = proto->readPackage();
	if (!p)
		return EReceive;
	if (p->getPackageType() == ID_ASCByePackage){
		delete p;
		return -1;
	}
	if (p->getPackageType() != ID_VSCSendValuePackage){
		delete p;
		return EProtocol;
	}
	
	p2 = proto->readPackage();
	if (!p2){
		delete p;
		return EReceive;
	}
	if (p->getPackageType() == ID_ASCByePackage){
		delete p;
		delete p2;
		return -1;
	}
	if (p2->getPackageType() != ID_VSCFinishedPackage){
		delete p;
		delete p2;
		return EProtocol;
	}
	
	delete p2;
	*res = ((VSCSendValuePackage*)p);
	return 0;
}


string LoximClient::LoximClient::ind_gen(int width)
{
    string res = "";
    for (int i = 0; i < width; i++)
	res += " ";
    return res;
}

void LoximClient::LoximClient::print_result(DataPart *part, int indent)
{
	int cnt;
	switch (part->getDataType()){
	    case DATAPART_TYPE_BAG:
		cout << ind_gen(indent) << "Bag" << endl;
		cnt = ((BagData*)part)->getCount();
		for (int i = 0; i < cnt; i++)
			print_result(((BagData*)part)->getDataParts()[i], indent + 2);
		break;
	    case DATAPART_TYPE_STRUCT:
		cout << ind_gen(indent) << "Sequence" << endl;
		cnt = ((StructData*)part)->getCount();
		for (int i = 0; i < cnt; i++)
			print_result(((StructData*)part)->getDataParts()[i], indent + 2);
		break;
	    case DATAPART_TYPE_SEQUENCE:
		cout << ind_gen(indent) << "Sequence" << endl;
		cnt = ((SequenceData*)part)->getCount();
		for (int i = 0; i < cnt; i++)
			print_result(((SequenceData*)part)->getDataParts()[i], indent + 2);
		break;
	    case DATAPART_TYPE_REFERENCE:
		cout << ind_gen(indent) << "ref(" << ((ReferenceData*)part)->getValue() << ")" << endl;
		break;
	    case DATAPART_TYPE_VARCHAR:
		cout << ind_gen(indent) << ((VarcharData*)part)->getValue()->getBuffor() << endl;
		break;
	    case DATAPART_TYPE_VOID:
		cout << ind_gen(indent) << "void" << endl;
		break;
	    case DATAPART_TYPE_SINT32:
		cout << ind_gen(indent) << ((Sint32Data*)part)->getValue() << endl;
		break;
	    case DATAPART_TYPE_DOUBLE:
		cout << ind_gen(indent) << ((DoubleData*)part)->getValue() << endl;
		break;
	    case DATAPART_TYPE_BOOL:
		cout << ind_gen(indent) << (((BoolData*)part)->getValue()?"true":"false") << endl;
		break;
	    case DATAPART_TYPE_BINDING:
		cout << ind_gen(indent) << ((BindingData*)part)->getBindingName()->getBuffor() << " => " << endl;
		print_result(((BindingData*)part)->getDataPart(), indent+2);
		break;
	    default:
		printf("Nieznany mi typ danych\n");	    
	}
}

int LoximClient::LoximClient::authorize(Authenticator *auth)
{
	Package *p=new WCHelloPackage(getpid(),CLIENT_NAME,CLIENT_VERSION,"localhost","PLN",0,0);
	if (!proto->writePackage(p))
	{
		printf("Hello nie posz�o\n");
		delete p;
		return -1;
	};
	delete p;
	
	p=proto->readPackage();
	if (!p)
	{
		printf("Hello nie przysz�o\n");
		delete p;
		return -2;
	}
	
	if (p->getPackageType()==ID_WSHelloPackage)
	{
		ws_hello=(WSHelloPackage*)p;			
	}else
	{
		return -3;
		delete p;
	}
	
	if  (!p)
		delete p;
		
	if (ws_hello->getAuthMethodsMap() && AUTH_TRUST)
	{
		int res=authTrust(new CharArray(strdup(auth->getLogin().c_str())));
		return res;
	};
	
	if (ws_hello->getAuthMethodsMap() && AUTH_PASS_MYSQL)
	{
		int res=authPassMySQL(
		    new CharArray(strdup(auth->getLogin().c_str())), 
		    new CharArray(strdup(auth->getPassword().c_str()))
		);
		return res;
	};
	
	return 1;
}




int LoximClient::LoximClient::authTrust(CharArray* user)
{
	Package *p=new WCLoginPackage(AUTH_TRUST);
	if (!proto->writePackage(p))
	{
		printf("Login: trust nie posz�o\n");
		delete p;
		return -1;
	};
	delete p;
	
	p=proto->readPackage();
	if(!p)
		return -2;
		
	if (p->getPackageType()!=ID_ASCOkPackage)
	{
		delete p;
		return -3;
	};
	delete p;
	
	
	p=new WCPasswordPackage(user,new CharArray(strdup("")));
	if (!proto->writePackage(p))
	{
		printf("Login: trust nie posz�o\n");
		delete p;
		return -4;
	};
	delete p;
	
	p=proto->readPackage();
	if(!p)
		return -5;
		
	if (p->getPackageType()!=ID_WSAuthorizedPackage)
	{
		delete p;
		return -6;
	};
	delete p;
	
	
	return 1;
};


int LoximClient::LoximClient::authPassMySQL(CharArray* user, CharArray* pass)
{
	Package *p=new WCLoginPackage(AUTH_TRUST);
	if (!proto->writePackage(p))
	{
		printf("Login: trust nie posz�o\n");
		delete p;
		return -1;
	};
	delete p;
	
	p=proto->readPackage();
	if(!p)
		return -2;
		
	if (p->getPackageType()!=ID_ASCOkPackage)
	{
		delete p;
		return -3;
	};
	delete p;
	
	
	p=new WCPasswordPackage(user, pass);
	if (!proto->writePackage(p))
	{
		printf("Login: trust nie posz�o\n");
		delete p;
		return -4;
	};
	delete p;
	
	p=proto->readPackage();
	if(!p)
		return -5;
		
	if (p->getPackageType()!=ID_WSAuthorizedPackage)
	{
		delete p;
		return -6;
	};
	delete p;
	
	
	return 1;
};

