
#include <iostream>
#include <sstream>
#include <vector>

#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>

#include <Protocol/Exceptions.h>
#include <Protocol/Enums/Enums.h>
#include <Protocol/Streams/TCPClientStream.h>
#include <Protocol/Streams/PackageCodec.h>
#include <Protocol/Packages/Package.h>
#include <Protocol/Packages/VSCSendvaluesPackage.h>
#include <Protocol/Packages/VSCSendvaluePackage.h>
#include <Protocol/Packages/VSCFinishedPackage.h>
#include <Protocol/Packages/ASCErrorPackage.h>
#include <Protocol/Packages/ASCByePackage.h>
#include <Protocol/Packages/ASCOkPackage.h>
#include <Protocol/Packages/WSAuthorizedPackage.h>
#include <Protocol/Packages/WCLoginPackage.h>
#include <Protocol/Packages/WCHelloPackage.h>
#include <Protocol/Packages/WSHelloPackage.h>
#include <Protocol/Packages/WCPasswordPackage.h>
#include <Protocol/Packages/QCStatementPackage.h>
#include <Errors/Errors.h>
#include <Client/SimpleClient.h>

#define MAX_PACKAGE_SIZE 100000


namespace Client {


const std::string SimpleClient::version = "0.99";

enum {
	ECliUnknown =		0x7000,
	ECliNotConnected =	0x7001,
	ECliParameter =		0x7002,
	ECliConnect =		0x7003,
	ECliAuth =		0x7004,
	ECliIO =		0x7005,
};


vector<string> tokenize2(const string &str, const string& dels = " ")
{
	vector<string> v;
	string::size_type pos = str.find_first_of(dels, 0);
	if (pos != string::npos) {
		v.push_back(str.substr(0, pos));
		v.push_back(str.substr(pos+1, str.length() - pos));
	} else {
		v.push_back(str);
		v.push_back("");
	}
	return v;
}

vector<string> tokenize(const string &str, const string& dels = " ")
{
	vector<string> v;
	string::size_type pos = 0, last = 0;
	while (true) {
		last = str.find_first_not_of(dels, pos);
		pos = str.find_first_of(dels, last);
		if (pos == string::npos && last == string::npos)
			break;
		else
			v.push_back(str.substr(last, pos-last));
	}
	return v;
}

vector<string> setvars(const string &s, vector<string> names, vector<string*> vars)
{
	vector<string> invalid;
	vector<string> tokens = tokenize(s, " "), t2;
	vector<string>::iterator it;
	for (it = tokens.begin(); it != tokens.end(); ++it) {
		if (*it == "")
			continue;
		t2 = tokenize2(*it,  "=");
		string key = t2[0], val = t2[1];
		vector<string>::size_type i;
		for (i = 0; i < names.size(); i++)
			if (names[i] == key)
				break;
		if (i < names.size())
			*vars[i] = val;
		else
			invalid.push_back(key);
	}
	return invalid;
}

uint32_t get_host_ip(const std::string& hostname)
{
	struct hostent *hp;
	hp = gethostbyname(hostname.c_str());
	if (hp == NULL)
		return 0;
	uint32_t res;
	memcpy(&res, hp->h_addr, 4);
	return res;
}
const std::string get_hostname()
{
	int size = 100;
	char *buf = new char[size];
	gethostname(buf, size);
	buf[size - 1] = 0;
	return string(buf);
}

void SimpleClient::initialize()
{
	connected = false;
	begin = 0;

	pthread_sigmask(0, NULL, &mask);
	sigaddset(&mask, SIGUSR1);
	shutting_down = false;
}

SimpleClient::SimpleClient(const std::string &host, int port,
		const std::string &login, const std::string &password)
{
	initialize();

	this->host = host;
	this->port = port;
	this->login = login;
	this->password = password;
}

SimpleClient::SimpleClient(const std::string &params)
{
	initialize();

	char *user = getenv("USER");
	this->host = "localhost";
	this->port = 2000;
	this->login = user != NULL ? user : "";
	this->password = "";

	vector<string> invalid;
	std::string portstring;
	vector<string> names;
	vector<string*> values;
	names.push_back("host"); values.push_back(&host);
	names.push_back("port"); values.push_back(&portstring);
	names.push_back("user"); values.push_back(&login);
	names.push_back("password"); values.push_back(&password);
	invalid = setvars(params, names, values);
	if (!invalid.empty()) {
		throw ErrorException(ECliParameter, "invalid parameter: " + invalid.front());
		return;
	}
	std::istringstream is(portstring);
	if (portstring != "" && !(is >> port)) {
		throw ErrorException(ECliParameter, "invalid port value: " + portstring);
		return;
	}
//std::cerr << "SC params: " << host << "/" << port << "/" << login << "/" << password << "/" << std::endl;
}

SimpleClient::~SimpleClient()
{
	if (connected)
		do_disconnect(true);
}

void SimpleClient::connect()
{
//std::cerr << "SC connect: " << host << ":" << port << std::endl;
	uint32_t haddr = get_host_ip(host.c_str());
	if (haddr == 0)
		throw ErrorException(ECliConnect, "unknown host: " + host);

	char buf[100];
	hostaddr = "???";
	if (inet_ntop(AF_INET, &haddr, buf, sizeof(buf)) != NULL)
		hostaddr = buf;

	std::auto_ptr<Protocol::PackageStream> s;
	try {
		s = std::auto_ptr<Protocol::PackageCodec>(new Protocol::PackageCodec(auto_ptr<Protocol::DataStream>(new Protocol::TCPClientStream(haddr, htons(port))), MAX_PACKAGE_SIZE));
	} catch (Protocol::ConnectionError &e) {
		throw ErrorException(ECliConnect, std::string("unable to connect: ") + strerror(e.get_error() & 0xFF));
	}
	stream = s;

	bool auth = authorize(login, password);
	if (!auth) {
		do_disconnect(true);
		throw ErrorException(ECliAuth, "authorization error");
	}

	connected = true;
	begin = time(NULL);
}

void SimpleClient::disconnect()
{
//std::cerr << "SC disconnect..." << std::endl;
	if (!connected)
		throw ErrorException(ECliNotConnected, "not connected");
	do_disconnect(true);
}

void SimpleClient::do_disconnect(bool send)
{
	auto_ptr<Protocol::ByteBuffer> buf(new Protocol::ByteBuffer("quit"));
	Protocol::ASCByePackage package(0, buf);
	try {
		if (send)
			stream->write_package(mask, shutting_down, package);
	} catch (Protocol::WriteError&) {
	}

	stream.reset();
	connected = false;
	begin = 0;
}

void SimpleClient::executeParams(const std::string &query,
	const Protocol::Package *&result, int nparams,
	const char * const *keys, const char * const *values,
	const int *lengths)
{
	throw ErrorException(ECliUnknown, "not implemented");
}

void SimpleClient::execute(const std::string &query,
		const Protocol::Package *&result)
{
	result = NULL;

//std::cerr << "SCL execute: " << query << std::endl;
	if (!connected)
		throw ErrorException(ECliNotConnected, "not connected");

	auto_ptr<Protocol::ByteBuffer> buf(new Protocol::ByteBuffer(query));
	Protocol::QCStatementPackage pack_query(Protocol::SF_EXECUTE, buf);
	try {
		stream->write_package(mask, shutting_down, pack_query);
	} catch (Protocol::WriteError&) {
		// workaround for next write locking up after server SIGSEGVs
		do_disconnect(false);
		throw ErrorException(ECliIO, "error sending query");
	}

	std::auto_ptr<Protocol::VSCSendvaluePackage> pack_answ;
	std::auto_ptr<Protocol::ASCErrorPackage> pack_err;

	bool bye;
	bye = receive_response(pack_answ, pack_err);
	if (pack_err.get() != NULL) {
		const Protocol::ByteBuffer &bb=pack_err->get_val_description();
		int size = bb.get_size();
		char *tmp = new char[size+1];
		memcpy(tmp, bb.get_const_data(), size);
		tmp[size] = '\0';
		std::string msg = tmp;
		delete[](tmp);

		int code = pack_err->get_val_error_code();
		throw ErrorException(code, msg);
	}
	if (pack_answ.get() == NULL)
		throw ErrorException(ECliIO, "error receiving response");

	result = &pack_answ->get_val_data();
	last_package = pack_answ;
}

bool SimpleClient::receive_response(
		std::auto_ptr<Protocol::VSCSendvaluePackage> &answer,
		std::auto_ptr<Protocol::ASCErrorPackage> &error)
{
	answer.reset();
	error.reset();

	std::auto_ptr<Protocol::Package> pack;
	std::auto_ptr<Protocol::VSCSendvaluePackage> svpack;

	pack = stream->read_package(mask, shutting_down);
	if (pack->get_type() == Protocol::A_SC_ERROR_PACKAGE) {
		error = std::auto_ptr<Protocol::ASCErrorPackage>
			(dynamic_cast<Protocol::ASCErrorPackage*> (pack.release()));
		return false;
	}
	if (pack->get_type() == Protocol::A_SC_BYE_PACKAGE)
		return true;
	if (pack->get_type() != Protocol::V_SC_SENDVALUES_PACKAGE)
		return false;

	pack = stream->read_package(mask, shutting_down);
	if (pack->get_type() == Protocol::A_SC_BYE_PACKAGE)
		return true;
	if (pack->get_type() != Protocol::V_SC_SENDVALUE_PACKAGE)
		return false;
	svpack = std::auto_ptr<Protocol::VSCSendvaluePackage>
		(dynamic_cast<Protocol::VSCSendvaluePackage*> (pack.release()));

	pack = stream->read_package(mask, shutting_down);
	if (pack->get_type() == Protocol::A_SC_BYE_PACKAGE)
		return true;
	if (pack->get_type() != Protocol::V_SC_FINISHED_PACKAGE)
		return false;

	answer = svpack;
	return false;
}


bool SimpleClient::authorize(const std::string &login, const std::string &password)
{
	auto_ptr<Protocol::ByteBuffer> cl_name(new Protocol::ByteBuffer("liblx"));
	auto_ptr<Protocol::ByteBuffer> cl_ver(new Protocol::ByteBuffer(version.c_str()));
	auto_ptr<Protocol::ByteBuffer> cl_hname(new Protocol::ByteBuffer(get_hostname()));
	auto_ptr<Protocol::ByteBuffer> cl_lang(new Protocol::ByteBuffer("PL"));
	Protocol::WCHelloPackage hello_s(getpid(), cl_name, cl_ver, cl_hname, cl_lang, Protocol::COLL_DEFAULT, 1);
	auto_ptr<Protocol::Package> hello_r;

	try {
		stream->write_package(mask, shutting_down, hello_s);
		hello_r = stream->read_package_expect(mask, shutting_down, Protocol::W_S_HELLO_PACKAGE);
	} catch (Protocol::ProtocolLogic&) {
		return false;
	} catch (Protocol::WriteError&) {
		return false;
	}

	if (dynamic_cast<Protocol::WSHelloPackage&>(*hello_r).get_val_auth_methods() & Protocol::AM_TRUST)
		return authTrust(login);

	return false;
}

bool SimpleClient::authTrust(const std::string &user)
{
	try {
		Protocol::WCLoginPackage p1(Protocol::AM_TRUST);
		stream->write_package(mask, shutting_down, p1);
		stream->read_package_expect(mask, shutting_down, Protocol::A_SC_OK_PACKAGE);

		auto_ptr<Protocol::ByteBuffer> login(new Protocol::ByteBuffer(user));
		auto_ptr<Protocol::ByteBuffer> passwd(new Protocol::ByteBuffer(user));
		Protocol::WCPasswordPackage p2(login, passwd);
		stream->write_package(mask, shutting_down, p2);
		stream->read_package_expect(mask, shutting_down, Protocol::W_S_AUTHORIZED_PACKAGE);

		return true;
	} catch (Protocol::ProtocolLogic&) {
	} catch (Protocol::WriteError&) {
	}
	return false;
}

int SimpleClient::statusConnected()
{
	return connected ? 1 : 0;
}

int SimpleClient::statusTime()
{
	return begin == 0 ? 0 : time(NULL) - begin;
}

std::string SimpleClient::statusHostname()
{
	return host;
}

std::string SimpleClient::statusHostaddr()
{
	return hostaddr;
}

int SimpleClient::statusPort()
{
	return port;
}

std::string SimpleClient::statusUser()
{
	return login;
}


}

