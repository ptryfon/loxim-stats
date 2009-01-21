#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <string>

namespace Client{
	class Authenticator{
	    public:
		Authenticator();
		virtual std::string get_login() = 0;
		virtual std::string get_password() = 0;
		
		//called by the client when login or password turn out to be wrong
		virtual void invalid() = 0;
		virtual void read() = 0;
		virtual ~Authenticator()= 0;
	};

}

#endif
