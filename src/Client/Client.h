#ifndef LOXIMCLIENT_H_
#define LOXIMCLIENT_H_

#include <Protocol/Packages/Package.h>
#include <Protocol/Streams/PackageStream.h>
#include <Protocol/Packages/ASCErrorPackage.h>
#include <Client/Authenticator.h>
#include <Util/Concurrency.h>
#include <Util/SignalReceiver.h>
#include <Util/InfLoopThread.h>

#include <string>
#include <memory>

namespace Client{

	class ProtocolThread;

	class OnExit
	{
		public:
			virtual void exit(int error) = 0;
			virtual ~OnExit();
	};

	class Client
	{
		friend class ProtocolThread;
	
		private:
			std::auto_ptr<Protocol::PackageStream> stream;

			Util::Mutex logic_mutex;
			Util::CondVar read_cond;
			int error;
			bool shutting_down;
			sigset_t mask;
			bool waiting_for_result;
			bool alive;
			std::auto_ptr<Protocol::Package> result;
			std::auto_ptr<Util::InfLoopThread<ProtocolThread> > proto_thread;
			OnExit *on_exit;
			
			void exit(int error, bool send_bye) throw ();
			void authorize(Authenticator&);
			void send_bye();
			void init(Authenticator &);
			std::auto_ptr<Protocol::Package> receive_result();
			void auth_trust(const std::string &user);
			void auth_pass_MySQL(const std::string &user, const std::string &password);
			std::string get_hostname();

		public:
			/* in network order */
			Client(uint32_t address, uint16_t port, Authenticator &auth);
			Client(uint32_t address, uint16_t port, Authenticator &auth, OnExit &on_exit);
			~Client();
			//may return null as a result of an abort
			std::auto_ptr<Protocol::Package> execute_stmt(const string &stmt);
			void abort();
	};
}

#endif /*LOXIMCLIENT_H_*/
