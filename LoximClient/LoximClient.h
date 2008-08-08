#ifndef LOXIMCLIENT_H_
#define LOXIMCLIENT_H_

#include <protocol/sockets/AbstractSocket.h>
#include <protocol/layers/ProtocolLayer0.h>
#include <protocol/packages/WSHelloPackage.h>
#include <protocol/ptools/CharArray.h>
#include <protocol/packages/data/DataPart.h>
#include <protocol/packages/ASCErrorPackage.h>
#include <protocol/packages/VSCSendValuePackage.h>
#include "StatementProvider.h"
#include "Authenticator.h"
#include "Aborter.h"

#include <string.h>

using namespace protocol;

namespace LoximClient{

	void *result_handler_starter(void *a);
	void signal_handler(int sig);
	class Aborter;

	class LoximClient
	{
		friend void *result_handler_starter(void *a);
		friend void signal_handler(int sig);
		private:
		static LoximClient *instance;
		/*Ustawiane w konstruktorze*/
		char* hostname;
		int port;
		
		/*Ustawianie w connect()*/
		AbstractSocket *socket;
		ProtocolLayer0 *proto;

		/*Ustawiane w run()*/
		pthread_mutex_t logic_mutex;
		pthread_cond_t read_cond;

		/*Ustawiane w authorize()*/
		WSHelloPackage *ws_hello;

		struct sigaction old_action;



		public:
		LoximClient(const char* a_hostname, int a_port);
		~LoximClient();

		int run(StatementProvider *provider);
		/**
		 * 
		 * >0 - ok
		 * <0 - b³±d
		 */
		int connect();

		/**
		 * >0 - ok
		 * <0 - b³±d
		 */
		int authorize(Authenticator *auth);
		int send_statement();
		void abort();

		Aborter *aborter;
		protected:
		int error;
		bool waiting_for_result;
		int receive_result(VSCSendValuePackage **, ASCErrorPackage **);
		void result_handler();
		int authTrust(CharArray* user);
		int authPassMySQL(CharArray* user, CharArray* password);
		string ind_gen(int width);
		void print_result(DataPart *part, int indent);
		void print_error(ASCErrorPackage *package);
		void register_handler();
		void unregister_handler();
	};
}

#endif /*LOXIMCLIENT_H_*/
