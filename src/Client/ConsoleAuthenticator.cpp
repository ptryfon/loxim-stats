#include <Client/ConsoleAuthenticator.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <sys/ioctl.h>
#include <termios.h>
#include <cstdlib>


//Linux needs it
#ifndef TIOCGETA
#define TIOCGETA TCGETA
#define TIOCSETA TCSETA
#endif 

using namespace std;

namespace Client{
	void ConsoleAuthenticator::read()
	{
		struct termios lock;
		struct termios save;
		char *buf;

		if (ask == ASK_FOR_BOTH){
			buf = readline("Login: ");
			if (!buf)
				return;
			login = string(buf);
			free(buf);
		} 
		if (ask == ASK_FOR_BOTH || ask == ASK_FOR_PASSWORD){
			tcgetattr(0, &lock);
			save = lock;

			lock.c_iflag &= ~(BRKINT);
			lock.c_lflag &= ~(ECHO|ISIG);
			ioctl(0, TIOCSETA, &lock);

			buf = readline("Password: ");
			tcsetattr(0, 0, &save);
			if (!buf)
				return;
			password = string(buf);
			free(buf);
		}
	}

	ConsoleAuthenticator::ConsoleAuthenticator() :
		ask(ASK_FOR_BOTH)
	{
	}

	
	ConsoleAuthenticator::ConsoleAuthenticator(const string &login, const string &password) :
		login(login), password(password), ask(0)
	{
	}
	
	ConsoleAuthenticator::ConsoleAuthenticator(const string &login) :
		login(login), ask(ASK_FOR_PASSWORD)
	{
	}

	
	void ConsoleAuthenticator::invalid()
	{
		ask = ASK_FOR_BOTH;
		read();
	}
	
	
	string ConsoleAuthenticator::get_login()
	{
		return login;
	}
	
	string ConsoleAuthenticator::get_password()
	{
		return password;
	}
	
	ConsoleAuthenticator::~ConsoleAuthenticator()
	{
	}
}

