#ifndef LOXIMCLIENT_H_
#define LOXIMCLIENT_H_

#include <Protocol/Packages/Package.h>
#include <Protocol/Streams/PackageStream.h>
#include <Protocol/Packages/ASCErrorPackage.h>
#include <Client/StatementProvider.h>
#include <Client/Authenticator.h>
#include <Client/Aborter.h>

#include <string>
#include <memory>

namespace Client{

	void *result_handler_starter(void *a);
	void signal_handler(int sig);
	class Aborter;

	class Client
	{
		friend void *result_handler_starter(void *a);
		friend void signal_handler(int sig);
	
		private:
			std::auto_ptr<Protocol::PackageStream> stream;

			pthread_mutex_t logic_mutex;
			pthread_cond_t read_cond;
			int error;
			bool shutting_down;
			sigset_t mask;
			bool waiting_for_result;
		public:
			Aborter aborter;
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
