#include "ConsoleAuthenticator.h"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <termio.h>

using namespace std;

namespace LoximClient{
	void ConsoleAuthenticator::read()
	{
		struct  termio  lock;
		struct  termio  save;
		char *buf;

		if (ask == ASK_FOR_BOTH){
			buf = readline("Login: ");
			if (!buf)
				return;
			login = string(buf);
			free(buf);
		} 
		if (ask == ASK_FOR_BOTH || ask == ASK_FOR_PASSWORD){
			ioctl(0, TCGETA, &lock);
			ioctl(0, TCGETA, &save);

			lock.c_iflag &= ~(BRKINT);
			lock.c_lflag &= ~(ECHO|ISIG);
			ioctl(0, TCSETA, &lock);

			buf = readline("Password: ");
			ioctl(0, TCSETA, &save);
			if (!buf)
				return;
			password = string(buf);
			free(buf);
		}
	}

	ConsoleAuthenticator::ConsoleAuthenticator()
	{
		ask = ASK_FOR_BOTH;
	}

	
	ConsoleAuthenticator::ConsoleAuthenticator(const string &login, const string &password)
	{
		this->login = login;
		this->password = password;
		ask = 0;
	}
	
	ConsoleAuthenticator::ConsoleAuthenticator(const string &login)
	{
		ask = ASK_FOR_PASSWORD;
		this->login = login;
	}

	ConsoleAuthenticator::ConsoleAuthenticator(const char *login, const char *password)
	{
		this->login = string(login);
		this->password = string(password);
		ask = 0;
	}

	ConsoleAuthenticator::ConsoleAuthenticator(const char *login)
	{
		this->login = string(login);
		ask = ASK_FOR_PASSWORD;
	}

	
	void ConsoleAuthenticator::invalid()
	{
		ask = ASK_FOR_BOTH;
		read();
	}
	
	
	string ConsoleAuthenticator::getLogin()
	{
		return login;
	}
	
	string ConsoleAuthenticator::getPassword()
	{
		return password;
	}
	
	ConsoleAuthenticator::~ConsoleAuthenticator()
	{
	}
}

