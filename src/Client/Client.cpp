#include <string>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <config.h>
#include <netdb.h>
#include <readline/readline.h>
#include <Client/Client.h>
#include <Client/Authenticator.h>
#include <Errors/Errors.h>
#include <Errors/Exceptions.h>
#include <Util/smartptr.h>
#include <Util/InfLoopThread.h>
#include <Util/Misc.h>
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
	class ProtocolThread : public Util::StartableSignalReceiver {
		friend class Client;
		private:
		Client &client;
		sigset_t mask;
		int error;
		public:

		ProtocolThread(Client &c) : client(c), error(0)
		{
			pthread_sigmask(0, NULL, &mask);
			sigaddset(&mask, SIGUSR1);
		}

		//call under mutex
		void gentle_exit(int error, bool send_bye)
		{
			this->error = error;
			client.shutting_down = true;
			client.waiting_for_result = false;
			client.exit(error, send_bye);
			client.read_cond.signal();
		}

		void start()
		{
			try {
				while(true){
					auto_ptr<Package> result = client.receive_result();
					Mutex::Locker l(client.logic_mutex);
					if (result->get_type() == A_SC_BYE_PACKAGE) {
						gentle_exit(0, true);
						return;
					}
					if (client.waiting_for_result && (
							result->get_type() ==
							A_SC_ERROR_PACKAGE ||
							result->get_type() ==
							V_SC_SENDVALUE_PACKAGE)) {
						client.waiting_for_result = false;
						client.result = result;
						client.read_cond.signal();
					} else {
						gentle_exit(EProtocol, false);
						return;
					}
				}
			} catch (OperationCancelled &ex) {
				gentle_exit(0, true);
			} catch (LoximException &ex) {
				gentle_exit(ex.get_error(), false);
			} catch (...) {
				gentle_exit(EUnknown, false);
			}
		}

		void kill()
		{
			pthread_kill(thread, SIGUSR1);
		}

		void signal_handler(int /*i*/)
		{
			client.shutting_down = true;
		}
	};

	OnExit::~OnExit()
	{
	}

	void Client::init(Authenticator &auth)
	{
		pthread_sigmask(0, NULL, &mask);
		sigaddset(&mask, SIGUSR1);
		authorize(auth);
		auto_ptr<ProtocolThread> proto_thread_obj(new ProtocolThread(*this));
		proto_thread = auto_ptr<InfLoopThread<ProtocolThread> >(new
				InfLoopThread<ProtocolThread>(proto_thread_obj,
					SIGUSR1));
	}

	Client::Client(uint32_t host, uint16_t port, Authenticator &auth) :
		stream(new PackageCodec(auto_ptr<DataStream>(new
		TCPClientStream(host, port)), MAX_PACKAGE_SIZE)), error(0),
		shutting_down(false), waiting_for_result(false), on_exit(NULL)
	{
		init(auth);
	}
	Client::Client(uint32_t host, uint16_t port, Authenticator &auth, OnExit
			&on_exit) :
		stream(new PackageCodec(auto_ptr<DataStream>(new
		TCPClientStream(host, port)), MAX_PACKAGE_SIZE)), error(0),
		shutting_down(false), waiting_for_result(false), on_exit(&on_exit)
	{
		init(auth);
	}

	Client::Client(const string & host, uint16_t port, Authenticator &auth) :
		stream(new PackageCodec(auto_ptr<DataStream>(new
		TCPClientStream(Client::get_host_ip(host), port)), MAX_PACKAGE_SIZE)), error(0),
		shutting_down(false), waiting_for_result(false), on_exit(NULL)
	{
		init(auth);
	}

	Client::Client(const string & host, uint16_t port, Authenticator &auth, OnExit
			&on_exit) :
		stream(new PackageCodec(auto_ptr<DataStream>(new
		TCPClientStream(Client::get_host_ip(host), port)), MAX_PACKAGE_SIZE)), error(0),
		shutting_down(false), waiting_for_result(false), on_exit(&on_exit)
	{
		init(auth);
	}

	Client::~Client()
	{
		//proto_thread touches client's properties, so we do this here
		//to be sure that they are still valid when the destruction
		//happens
		proto_thread.reset();
	}


	void Client::abort()
	{
		Mutex::Locker l(logic_mutex);
		if (waiting_for_result) {
			auto_ptr<ByteBuffer> reason(new ByteBuffer("User aborted"));
			VSCAbortPackage package(0, reason);
			stream->write_package(mask, shutting_down, package);
			waiting_for_result = false;
			result.reset();
			read_cond.signal();
		}
	}


	//called under mutex
	void Client::exit(int error, bool send_bye) throw()
	{
		if (send_bye){
			try {
				this->send_bye();
			} catch (...) {
				//don't care
			}
		}
		try {
			if (on_exit)
				on_exit->exit(error);
		} catch ( ... ) {
			//XXX log it
		}
	}


	//this can return ASCError, ASCBye or ASCSendValue
	auto_ptr<Package> Client::receive_result()
	{
		while (true){
			auto_ptr<Package> p(stream->read_package(mask, shutting_down));
			if (p->get_type() == A_SC_ERROR_PACKAGE || p->get_type() == A_SC_BYE_PACKAGE){
				return p;
			}
			if (p->get_type() == A_SC_PING_PACKAGE){
				Mutex::Locker l(logic_mutex);
				ASCPongPackage pong;
				stream->write_package(mask, shutting_down, pong);
				continue;
			}
			if (p->get_type() != V_SC_SENDVALUES_PACKAGE){
				throw ProtocolLogic();
			}

			p = stream->read_package(mask, shutting_down);
			if (p->get_type() == A_SC_PING_PACKAGE){
				Mutex::Locker l(logic_mutex);
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
				Mutex::Locker l(logic_mutex);
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
			auto_ptr<ByteBuffer> cl_ver(new ByteBuffer(STRINGIFY(PACKAGE_VERSION)));
			auto_ptr<ByteBuffer> cl_hname(new ByteBuffer(get_hostname()));
			//XXX use locale
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

	//may return null as a result of an abort
	auto_ptr<Package> Client::execute_stmt(const string &stmt)
	{
		Mutex::Locker l(logic_mutex);
		if (waiting_for_result)
			throw ProtocolLogic();
		auto_ptr<ByteBuffer> b_stmt(new ByteBuffer(stmt));
		QCStatementPackage package(SF_EXECUTE, b_stmt);
		stream->write_package(mask, shutting_down,
				package);
		waiting_for_result = true;
		l.wait(read_cond);
		//if result == NULL then it is an effect of abort
		if (proto_thread->get_logic().error)
			throw LoximException(proto_thread->get_logic().error);
		return result;
	}

	void Client::send_bye()
	{
		//shutting_down is almoast for sure true now, so let's override
		//it with sd
		bool sd = false;
		auto_ptr<ByteBuffer> buf(new ByteBuffer( "Client quitted"));
		ASCByePackage package(0, buf);
		stream->write_package(mask, sd,	package);
		shutting_down = true;
		return;
	}

	/*in network order */
	uint32_t Client::get_host_ip(const string& hostname)
	{
		struct hostent *hp;
		hp = gethostbyname(hostname.c_str());
		if (!hp){
			throw LoximException(h_errno);
		}
		uint32_t res;
		memcpy(&res, hp->h_addr, 4);
		return res;
	}

}
