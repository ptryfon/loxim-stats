#ifndef CONSOLE_AUTHENTICATOR_H
#define CONSOLE_AUTHENTICATOR_H

#include <string>
#include "Authenticator.h"

using namespace std;

#define ASK_FOR_PASSWORD 1
#define ASK_FOR_BOTH 2

namespace LoximClient{
	class ConsoleAuthenticator : public Authenticator {
	    protected:
		string login;
		string password;
		int ask;
	    public:
		void read();
		ConsoleAuthenticator();
		ConsoleAuthenticator(const string &login);
		ConsoleAuthenticator(const string &login, const string &password);
		ConsoleAuthenticator(const char *login);
		ConsoleAuthenticator(const char *login, const char *password);
		string getLogin();
		string getPassword();
		void invalid();
		~ConsoleAuthenticator();
	};
}

#endif
