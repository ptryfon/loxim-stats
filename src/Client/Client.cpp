#include <string>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <readline/readline.h>
#include <Client/Client.h>
#include <Client/Authenticator.h>
#include <Errors/Errors.h>
#include <Errors/Exceptions.h>
#include <Util/Locker.h>
#include <Util/smartptr.h>
#include <Protocol/Exceptions.h>
#include <Protocol/Streams/TCPClientStream.h>
#include <Protocol/Enums/Enums.h>
#include <Protocol/Streams/PackageStream.h>
#include <Protocol/Streams/PackageCodec.h>
#include <Protocol/Packages/ASCByePackage.h>
#include <Protocol/Packages/ASCErrorPackage.h>
#include <Protocol/Packages/ASCOkPackage.h>
#include <Protocol/Packages/ASCPingPackage.h>
#include <Protocol/Packages/ASCPongPackage.h>
#include <Protocol/Packages/ASCSetoptPackage.h>
#include <Protocol/Packages/QCExecutePackage.h>
#include <Protocol/Packages/QCStatementPackage.h>
#include <Protocol/Packages/VSCAbortPackage.h>
#include <Protocol/Packages/VSCFinishedPackage.h>
#include <Protocol/Packages/VSCSendvaluePackage.h>
#include <Protocol/Packages/VSCSendvaluesPackage.h>
#include <Protocol/Packages/WCHelloPackage.h>
#include <Protocol/Packages/WCLoginPackage.h>
#include <Protocol/Packages/WCPasswordPackage.h>
#include <Protocol/Packages/WSAuthorizedPackage.h>
#include <Protocol/Packages/WSHelloPackage.h>
#include <Protocol/Packages/Data/BagPackage.h>
#include <Protocol/Packages/Data/BindingPackage.h>
#include <Protocol/Packages/Data/BoolPackage.h>
#include <Protocol/Packages/Data/DoublePackage.h>
#include <Protocol/Packages/Data/RefPackage.h>
#include <Protocol/Packages/Data/SequencePackage.h>
#include <Protocol/Packages/Data/Sint32Package.h>
#include <Protocol/Packages/Data/StructPackage.h>
#include <Protocol/Packages/Data/VarcharPackage.h>
#include <Protocol/Packages/Data/VoidPackage.h>



using namespace Protocol;
using namespace std;
using namespace Errors;
using namespace Util;
using namespace _smptr;

namespace Client{

	#define MAX_PACKAGE_SIZE 100000


	Client::Client(uint32_t host, uint16_t port) :
		stream(new PackageCodec(auto_ptr<DataStream>(new
		TCPClientStream(host, port)), MAX_PACKAGE_SIZE)), error(0),
		shutting_down(false), waiting_for_result(false),
		aborter(*this) 
	{
		pthread_mutex_init(&logic_mutex, 0);
		pthread_cond_init(&read_cond, 0);
		pthread_sigmask(0, NULL, &mask);
		sigaddset(&mask, SIGUSR1);

	}

	Client::~Client()
	{
	}


	void Client::run(StatementProvider &provider)
	{
		pthread_mutex_t cond_mutex;
		pthread_mutex_init(&cond_mutex, 0);

		pthread_t result_thread;
		pthread_create(&result_thread, 0, result_handler_starter, this);
		/* kill both on exit */
		//result_handler->start();
		aborter.start();
		while (true){
			string stmt = provider.read_stmt();
			{
				Locker l(logic_mutex);
				if (!stmt.compare("quit")){
					auto_ptr<ByteBuffer> buf(new ByteBuffer("Client quitted"));
					ASCByePackage package(0, buf);
					stream->write_package(mask, shutting_down, package);
					return;
				}
				{
					auto_ptr<ByteBuffer> b_stmt(new ByteBuffer(stmt));
					QCStatementPackage package(SF_EXECUTE, b_stmt);
					stream->write_package(mask, shutting_down, package);
				}
				waiting_for_result = true;
				pthread_cond_wait(&read_cond, &logic_mutex);
			}
		}
	}

	void Client::abort()
	{
		Locker l(logic_mutex);
		if (!waiting_for_result)
			printf("Nothing to abort\n");
		else{
			auto_ptr<ByteBuffer> reason(new ByteBuffer("User aborted"));
			VSCAbortPackage package(0, reason);
			stream->write_package(mask, shutting_down, package);
			waiting_for_result = false;
			pthread_cond_signal(&read_cond);
		}
	}

	void *result_handler_starter(void *a)
	{
		reinterpret_cast<Client*>(a)->result_handler();
		return NULL;
	}

	void Client::print_error(const ASCErrorPackage &package)
	{
		cout << "Got error " << package.get_val_error_code() << ": " <<
			package.get_val_description().to_string() << endl;
	}

	void Client::result_handler()
	{
		while(true){
			auto_ptr<Package> result = receive_result();
			//if error in protocol occurred or the server wants to close the connection
			{
				Locker l(logic_mutex);
				if (result->get_type() == A_SC_PING_PACKAGE){
					ASCPongPackage p;
					stream->write_package(mask, shutting_down, p);
					continue;
				}
				if (waiting_for_result){
					switch (result->get_type()){
						case A_SC_ERROR_PACKAGE:
							{
								print_error(dynamic_cast<ASCErrorPackage&>(*result));
								break;
							}
						case V_SC_SENDVALUE_PACKAGE:
							{
								print_result(dynamic_cast<VSCSendvaluePackage&>(*result).get_val_data(), 0);
								break;
							}
						case A_SC_BYE_PACKAGE:
							{
								//close client
								//connection
							}
						default:
							throw runtime_error("internal error");
					}
					error = 0;
					waiting_for_result = false;
				} else {
					//close client connection
					error = EProtocol;
				}
				pthread_cond_signal(&read_cond);
			}
		}
	}

	auto_ptr<Package> Client::receive_result()
	{
		while (true){
			auto_ptr<Package> p(stream->read_package(mask, shutting_down));
			if (p->get_type() == A_SC_ERROR_PACKAGE || p->get_type() == A_SC_BYE_PACKAGE){
				return p;
			}
			if (p->get_type() == A_SC_PING_PACKAGE){
				Locker l(logic_mutex);
				ASCPongPackage pong;
				stream->write_package(mask, shutting_down, pong);
				continue;
			}
			if (p->get_type() != V_SC_SENDVALUES_PACKAGE){
				throw ProtocolLogic();
			}

			p = stream->read_package(mask, shutting_down);
			if (p->get_type() == A_SC_PING_PACKAGE){
				Locker l(logic_mutex);
				ASCPongPackage pong;
				stream->write_package(mask, shutting_down, pong);
				continue;
			}
			if (p->get_type() == A_SC_BYE_PACKAGE){
				return p;
			}
			if (p->get_type() != V_SC_SENDVALUE_PACKAGE){
				throw ProtocolLogic();
			}

			auto_ptr<Package> p2(stream->read_package(mask, shutting_down));
			if (p->get_type() == A_SC_PING_PACKAGE){
				Locker l(logic_mutex);
				ASCPongPackage pong;
				stream->write_package(mask, shutting_down, pong);
				continue;
			}
			if (p2->get_type() != A_SC_BYE_PACKAGE && p2->get_type() != V_SC_FINISHED_PACKAGE){
				throw ProtocolLogic();
			}
			if (p2->get_type() == A_SC_BYE_PACKAGE)
				return p2;
			else
				return p;
		}
	}


	void Client::print_result(const Package &package, int ind)
	{
		switch (package.get_type()){
			case BAG_PACKAGE:
				{
					cout << string(ind, ' ') << "Bag" << endl;
					const BagPackage &bag(dynamic_cast<const BagPackage&>(package));
					const vector<shared_ptr<Package> > &items(bag.get_packages());
					for (vector<shared_ptr<Package> >::const_iterator i = items.begin(); i != items.end(); ++i)
						print_result(**i, ind+2);
				}
				break;
			case STRUCT_PACKAGE:
				{
					cout << string(ind, ' ') << "Struct" << endl;
					const StructPackage &strct(dynamic_cast<const StructPackage&>(package));
					const vector<shared_ptr<Package> > &items(strct.get_packages());
					for (vector<shared_ptr<Package> >::const_iterator i = items.begin(); i != items.end(); ++i)
						print_result(**i, ind+2);
				}
				break;
			case SEQUENCE_PACKAGE:
				{
					cout << string(ind, ' ') << "Sequence" << endl;
					const SequencePackage &seq(dynamic_cast<const SequencePackage&>(package));
					const vector<shared_ptr<Package> > &items(seq.get_packages());
					for (vector<shared_ptr<Package> >::const_iterator i = items.begin(); i != items.end(); ++i)
						print_result(**i, ind+2);
				}
				break;
			case REF_PACKAGE:
				cout << string(ind, ' ') << "ref(" << dynamic_cast<const RefPackage&>(package).get_val_value_id() << ")" << endl;
				break;
			case VARCHAR_PACKAGE:
				cout << string(ind, ' ') << dynamic_cast<const VarcharPackage&>(package).get_val_value().to_string() << endl;
				break;
			case VOID_PACKAGE:
				cout << string(ind, ' ') << "void" << endl;
				break;
			case SINT32_PACKAGE:
				cout << string(ind, ' ') << dynamic_cast<const Sint32Package&>(package).get_val_value() << endl;
				break;
			case DOUBLE_PACKAGE:
				cout << string(ind, ' ') << dynamic_cast<const DoublePackage&>(package).get_val_value() << endl;
				break;
			case BOOL_PACKAGE:
				cout << string(ind, ' ') << dynamic_cast<const BoolPackage&>(package).get_val_value() << endl;
				break;
			case BINDING_PACKAGE:
				{
					const BindingPackage &bp(dynamic_cast<const BindingPackage&>(package));
					cout << string(ind, ' ') << bp.get_val_binding_name().to_string() << "=>" << endl;
					print_result(bp.get_val_value(), ind + 2);
				}
				break;
			default:
				cout << string(ind, ' ') << "Unknown data type" << endl;
		}
	}

	string Client::get_hostname()
	{
		int size = 100;
		char *buf = new char[size];
		gethostname(buf, size);
		buf[size - 1] = 0;
		return string(buf);
	}

	void Client::authorize(Authenticator &auth)
	{
		{
			auto_ptr<ByteBuffer> cl_name(new ByteBuffer("lsbql"));
			//TODO, use autoconf
			auto_ptr<ByteBuffer> cl_ver(new ByteBuffer("0.01"));
			auto_ptr<ByteBuffer> cl_hname(new ByteBuffer(get_hostname()));
			auto_ptr<ByteBuffer> cl_lang(new ByteBuffer("PL"));
			WCHelloPackage hello(getpid(), cl_name, cl_ver, cl_hname, cl_lang, COLL_DEFAULT, 1);
			stream->write_package(mask, shutting_down, hello);
		}
		auto_ptr<Package> hello = stream->read_package_expect(mask, shutting_down, W_S_HELLO_PACKAGE);

		if (dynamic_cast<WSHelloPackage&>(*hello).get_val_auth_methods() & AM_TRUST)
		{
			auth_trust(auth.get_login());
			return;
		}

		if (dynamic_cast<WSHelloPackage&>(*hello).get_val_auth_methods() & AM_MYSQL)
		{
			auth_pass_MySQL(auth.get_login(), auth.get_password());
			return;
		};
		throw ProtocolLogic();
	}




	void Client::auth_trust(const string &user)
	{
		{
			WCLoginPackage p(AM_TRUST);
			stream->write_package(mask, shutting_down, p);
		}
		stream->read_package_expect(mask, shutting_down, A_SC_OK_PACKAGE);
		{
			auto_ptr<ByteBuffer> login(new ByteBuffer(user));
			auto_ptr<ByteBuffer> passwd(new ByteBuffer(user));
			WCPasswordPackage p(login, passwd);
			stream->write_package(mask, shutting_down, p);
		}
		stream->read_package_expect(mask, shutting_down, W_S_AUTHORIZED_PACKAGE);
	}


	//Just a stub, it doesn't work
	void Client::auth_pass_MySQL(const string &user, const string &passwd)
	{
		{
			WCLoginPackage p(AM_MYSQL);
			stream->write_package(mask, shutting_down, p);
		}
		stream->read_package_expect(mask, shutting_down, A_SC_OK_PACKAGE);
		{
			auto_ptr<ByteBuffer> login(new ByteBuffer(user));
			auto_ptr<ByteBuffer> bpasswd(new ByteBuffer(passwd));
			WCPasswordPackage p(login, bpasswd);
			stream->write_package(mask, shutting_down, p);
		}
		stream->read_package_expect(mask, shutting_down, W_S_AUTHORIZED_PACKAGE);
	}
}
