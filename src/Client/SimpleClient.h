#ifndef SIMPLECLIENT_H_
#define SIMPLECLIENT_H_

#include <string>
#include <memory>

namespace Protocol {
	class PackageStream;
	class VSCSendvaluePackage;
	class ASCErrorPackage;
	class Package;
}

namespace Client {


class ErrorException {
protected:
	int code;
	std::string msg;
	int syserr;
	std::string sysmsg;
public:
	ErrorException(int code, const std::string &msg) : code(code), msg(msg), syserr(0) {}
	ErrorException(int code, const std::string &msg, int sys) : code(code), msg(msg), syserr(sys) { sysmsg = strerror(syserr); }
	virtual ~ErrorException() {}

	int Code() { return code; }
	std::string& Msg() { return msg; }
	int Syserr() { return syserr; }
	std::string& Sysmsg() { return sysmsg; }
};

class SimpleClient {
private:
	std::string host;
	int port;
	std::string login;
	std::string password;
	std::string hostaddr;
	time_t begin;

	bool connected;
	sigset_t mask;
	bool shutting_down;
	std::auto_ptr<Protocol::PackageStream> stream;
	std::auto_ptr<Protocol::VSCSendvaluePackage> last_package;

	bool receive_response(
			std::auto_ptr<Protocol::VSCSendvaluePackage> &answer,
			std::auto_ptr<Protocol::ASCErrorPackage> &error);
	bool authorize(const std::string &login, const std::string &password);
	bool authTrust(const std::string &user);
	void initialize();
	void do_disconnect(bool send);
public:
	static const std::string version;

	SimpleClient(const std::string &params);
	SimpleClient(const std::string &host, int port,
			const std::string &login, const std::string &password);
	~SimpleClient();

	void connect();
	void disconnect();

	void execute(const std::string &query,
			const Protocol::Package *&result);
	void executeParams(const std::string &query,
			const Protocol::Package *&result,
			int nparams,
			const char* const *keys,
			const char* const *values,
			const int *lengths);

	int statusConnected();
	int statusTime();
	std::string statusHostname();
	std::string statusHostaddr();
	int statusPort();
	std::string statusUser();
};


}


#endif
