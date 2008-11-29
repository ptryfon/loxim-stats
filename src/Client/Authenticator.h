#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <string>

using namespace std;

namespace Client{
	class Authenticator{
	    public:
		Authenticator();
		virtual string getLogin() = 0;
		virtual string getPassword() = 0;
		
		//called by the client when login or password turn out to be wrong
		virtual void invalid() = 0;
		virtual void read() = 0;
		virtual ~Authenticator()= 0;
	};

}

#endif
