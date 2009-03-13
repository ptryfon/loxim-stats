#ifndef LOXIMCLIENT_H_
#define LOXIMCLIENT_H_

#include <Protocol/Packages/Package.h>
#include <Protocol/Streams/PackageStream.h>
#include <Protocol/Packages/ASCErrorPackage.h>
#include <Client/StatementProvider.h>
#include <Client/Authenticator.h>
#include <Client/Aborter.h>
#include <Util/Concurrency.h>
#include <Util/SignalReceiver.h>

#include <string>
#include <memory>

namespace Client{

	void *result_handler_starter(void *a);
	void signal_handler(int sig);
	class Aborter;

	class StatementReader : public Util::StartableSignalReceiver {
		public:
			StatementReader(Client &client, StatementProvider &provider);
			void start();
			void kill(bool synchronous);
			void signal_handler(int sig);
		protected:
			Util::Mutex mutex;
			Util::CondVar cond;
			bool shutting_down;
			Client &client;
			StatementProvider &provider;
	};

	class Client
	{
		friend void *result_handler_starter(void *a);
		friend void signal_handler(int sig);
		friend class StatementReader;
	
		private:
			std::auto_ptr<Protocol::PackageStream> stream;

			Util::Mutex logic_mutex;
			Util::CondVar read_cond;
			int error;
			bool shutting_down;
			sigset_t mask;
			bool waiting_for_result;
			
		private:
			
			std::auto_ptr<Protocol::Package> receive_result();
			void result_handler();
			void auth_trust(const std::string &user);
			void auth_pass_MySQL(const std::string &user, const std::string &password);
			void print_result(const Protocol::Package &package, int indent);
			void print_error(const Protocol::ASCErrorPackage &package);
			std::string get_hostname();

		public:
			/* in network order */
			Client(uint32_t address, uint16_t port);
			~Client();

			void run(StatementProvider &provider);

			void authorize(Authenticator &auth);
			
			void send_statement();
			void abort();

	};
}

#endif /*LOXIMCLIENT_H_*/
