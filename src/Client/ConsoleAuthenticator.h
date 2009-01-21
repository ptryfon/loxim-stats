#ifndef CONSOLE_AUTHENTICATOR_H
#define CONSOLE_AUTHENTICATOR_H

#include <string>
#include <Client/Authenticator.h>

using namespace std;


namespace Client{

	enum {
		ASK_FOR_PASSWORD = 1,
		ASK_FOR_BOTH 
	};

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
		string get_login();
		string get_password();
		void invalid();
		~ConsoleAuthenticator();
	};
}

#endif
