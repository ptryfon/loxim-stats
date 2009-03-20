#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>

#include <readline/readline.h>
#include <Client/Client.h>
#include <Client/ClientConsole.h>
#include <Client/ConsoleAuthenticator.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/Packages/ASCByePackage.h>
#include <Protocol/Packages/ASCErrorPackage.h>
#include <Protocol/Packages/ASCOkPackage.h>
#include <Protocol/Packages/ASCPingPackage.h>
#include <Protocol/Packages/ASCPongPackage.h>
#include <Protocol/Packages/ASCSetoptPackage.h>
#include <Protocol/Packages/QCExecutePackage.h>
#include <Protocol/Packages/QCStatementPackage.h>
#include <Protocol/Packages/VSCAbortPackage.h>
#include <Protocol/Packages/VSCFinishedPackage.h>
#include <Protocol/Packages/VSCSendvaluePackage.h>
#include <Protocol/Packages/VSCSendvaluesPackage.h>
#include <Protocol/Packages/WCHelloPackage.h>
#include <Protocol/Packages/WCLoginPackage.h>
#include <Protocol/Packages/WCPasswordPackage.h>
#include <Protocol/Packages/WSAuthorizedPackage.h>
#include <Protocol/Packages/WSHelloPackage.h>
#include <Protocol/Packages/Data/BagPackage.h>
#include <Protocol/Packages/Data/BindingPackage.h>
#include <Protocol/Packages/Data/BoolPackage.h>
#include <Protocol/Packages/Data/DoublePackage.h>
#include <Protocol/Packages/Data/RefPackage.h>
#include <Protocol/Packages/Data/SequencePackage.h>
#include <Protocol/Packages/Data/Sint32Package.h>
#include <Protocol/Packages/Data/StructPackage.h>
#include <Protocol/Packages/Data/VarcharPackage.h>
#include <Protocol/Packages/Data/VoidPackage.h>
#include <Util/smartptr.h>

#define DEFAULT_HOSTNAME "localhost"
#define DEFAULT_PORT 2000

using namespace Protocol;
using namespace Client;
using namespace _smptr;
using namespace Util;

bool shutting_down = false;
pthread_t thread;

void print_error(const ASCErrorPackage &package)
{
	cout << "Got error " << package.get_val_error_code() << ": " <<
		package.get_val_description().to_string() << endl;
}

void print_result(const Package &package, int ind)
{
	switch (package.get_type()){
		case BAG_PACKAGE:
			{
				cout << string(ind, ' ') << "Bag" << endl;
				const BagPackage &bag(dynamic_cast<const
						BagPackage&>(package));
				const vector<shared_ptr<Package> >
					&items(bag.get_packages());
				for (vector<shared_ptr<Package> >::const_iterator
						i = items.begin();
						i != items.end();
						++i)
					print_result(**i, ind+2);
			}
			break;
		case STRUCT_PACKAGE:
			{
				cout << string(ind, ' ') << "Struct" << endl;
				const StructPackage &strct(dynamic_cast<const
						StructPackage&>(package));
				const vector<shared_ptr<Package> >
					&items(strct.get_packages());
				for (vector<shared_ptr<Package> >::const_iterator
						i = items.begin();
						i != items.end();
						++i)
					print_result(**i, ind+2);
			}
			break;
		case SEQUENCE_PACKAGE:
			{
				cout << string(ind, ' ') << "Sequence" << endl;
				const SequencePackage &seq(dynamic_cast<const
						SequencePackage&>(package));
				const vector<shared_ptr<Package> >
					&items(seq.get_packages());
				for (vector<shared_ptr<Package> >::const_iterator
						i = items.begin();
						i != items.end();
						++i)
					print_result(**i, ind+2);
			}
			break;
		case REF_PACKAGE:
			cout << string(ind, ' ') << "ref(" <<
				dynamic_cast<const
				RefPackage&>(package).get_val_value_id() << ")"
				<< endl;
			break;
		case VARCHAR_PACKAGE:
			cout << string(ind, ' ') << dynamic_cast<const
				VarcharPackage&>(package).get_val_value().to_string()
				<< endl;
			break;
		case VOID_PACKAGE:
			cout << string(ind, ' ') << "void" << endl;
			break;
		case SINT32_PACKAGE:
			cout << string(ind, ' ') << dynamic_cast<const
				Sint32Package&>(package).get_val_value() <<
				endl;
			break;
		case DOUBLE_PACKAGE:
			cout << string(ind, ' ') << dynamic_cast<const
				DoublePackage&>(package).get_val_value() <<
				endl;
			break;
		case BOOL_PACKAGE:
			cout << string(ind, ' ') << dynamic_cast<const
				BoolPackage&>(package).get_val_value() << endl;
			break;
		case BINDING_PACKAGE:
			{
				const BindingPackage &bp(dynamic_cast<const
						BindingPackage&>(package));
				cout << string(ind, ' ') <<
					bp.get_val_binding_name().to_string()
					<< "=>" << endl;
				print_result(bp.get_val_value(), ind + 2);
			}
			break;
		default:
			cout << string(ind, ' ') << "Unknown data type" <<
				endl;
	}
}

void print_usage(const string &program_name, int exit_code)
{
	cout << "LoXiM client" << endl;
	cout << "Usage: " << program_name << " [opts]" << endl;
	cout << "             --help           Print this information" << endl;
	cout << " -h [addr]   --host [addr]    Set the address of the server (" << DEFAULT_HOSTNAME << " by default)" << endl;
	cout << " -p [port]   --port [port]    Set the destination port (" << DEFAULT_PORT << "by default)" << endl;
	cout << " -l [login]  --login [login]  Set the login" << endl;
	cout << " -P [pwd]    --password [pwd] Set the password" << endl;
	cout << " -m [mode]   --mode [mode]    Iinital mode of input (dot|slash) (slash by default)" << endl;
	cout << " -f [file]   --file [file]    Read querries from file" << endl;
	exit(exit_code);
};

bool is_number(char *n)
{
	for (;*n; n++)
		if (!isdigit(*n))
			return false;
	return true;
}

/*in network order */
uint32_t get_host_ip(const string& hostname)
{
	struct hostent *hp;
	hp = gethostbyname(hostname.c_str());
	if (!hp){
		cout << "Could resolve hostname " << hostname;
		exit(EINVAL);
	}
	uint32_t res;
	memcpy(&res, hp->h_addr, 4);
	return res;
}

class Aborter : public SignalReceiver {
	::Client::Client &client;

	public:
	Aborter(::Client::Client &client) : client(client) {};
	void signal_handler(int sig)
	{
		switch (sig){
			case SIGINT:
				client.abort();
				break;
		}
		
	}
	~Aborter()
	{
		SignalRouter::unregister_thread(pthread_self());
	}
};

class LsbqlOnExit : public OnExit {
	void exit(int error)
	{
		if (error) {
			cout << "Connection error" << endl;
		} else {
			cout << "Server disconnected" << endl;
		}
		if (!shutting_down)
			pthread_kill(thread, SIGTERM);
	}
};

int main(int argc, char** argv)
{
	printf("Welcome to loxim client.\n\n");
	
	const char* short_opt ="h:p:l:P:f:m:";
	const struct option long_options[] =
	{
		{"help",	0,	NULL,	'H'},
		{"host",	1,	NULL,	'h'},
		{"port",	1,	NULL,	'p'},
		{"login",	1,	NULL,	'l'},
		{"password",	1,	NULL,	'P'},
		{"file",	1,	NULL,	'f'},
		{"mode",	1,	NULL,	'm'},
		{NULL,		0,	NULL,	0}
	};
	string login, password, hostname = DEFAULT_HOSTNAME;
	uint16_t port=DEFAULT_PORT;
	int mode = CC_DOT;
	auto_ptr<ifstream> file;
	int next_option;
	do {
		next_option=getopt_long(argc,argv,short_opt, long_options,NULL);
		switch (next_option)
		{
			case 'h':
				hostname=optarg;
				break;
				
			case 'p':
				if (!is_number(optarg)){
					cout << "Invalid port number" << endl;
					exit(EINVAL);
				} else {
					port= atoi(optarg);
				}
				break;
				
			case 'H':
				print_usage(argv[0],0);
				break;

			case 'l':
				login = optarg;
				break;

			case 'P':
				password = optarg;
				break;
			case 'f':
				file = auto_ptr<ifstream>(new ifstream(optarg));
				if (!file->is_open()){
					cout << "Cannot open file " << optarg << endl;
					cout << "Error " << errno << ": (" <<
						strerror(errno) << ")" << endl;
					exit(EINVAL);
				}
				break;

			case 'm':
				if (!strcmp(optarg, "dot")){
					mode = CC_DOT;
					break;
				}
				if (!strcmp(optarg, "slash")){
					mode = CC_SLASH;
					break;
				}
				print_usage(argv[0], -1);
			case -1:
				break;
				
			default:
				print_usage(argv[0],-1);
				break;
		}
	}while (next_option != -1);



	auto_ptr<ConsoleAuthenticator> auth;
	if (login.size() && password.size())
		auth = auto_ptr<ConsoleAuthenticator>(new
				ConsoleAuthenticator(login, password));
	else
		if (login.size())
			auth = auto_ptr<ConsoleAuthenticator>(new
					ConsoleAuthenticator(login));
		else
			auth = auto_ptr<ConsoleAuthenticator>(new
					ConsoleAuthenticator());
	auth->read();
	thread = pthread_self();
	LsbqlOnExit on_exit;
	::Client::Client client(get_host_ip(hostname), htons(port), *auth, on_exit);
	Aborter aborter(client);
	SignalRouter::register_thread(aborter, SIGINT);
	cout << "Connected." << endl;

	if (isatty(0))
		cout << "Type $help to see the available commands" << endl;
	cout << "Authorized." << endl;

	auto_ptr<ClientConsole> cc;
	if (file.get())
		cc = auto_ptr<ClientConsole>(new ClientConsole(mode, file));
	else
		cc = auto_ptr<ClientConsole>(new ClientConsole(mode));
	while (true){
		string stmt =
			cc->read_stmt();
		if (shutting_down)
			return 0;
		if (!stmt.compare("quit")){
			shutting_down = true;
			return 0; 
		}
		auto_ptr<Package> result =
			client.execute_stmt(stmt);
		if (!result.get())
			cout << "Aborted" << endl;
		else {
			if (result->get_type() ==
					A_SC_ERROR_PACKAGE)
				print_error(dynamic_cast<ASCErrorPackage&>(
							*result));
			if (result->get_type() ==
					V_SC_SENDVALUE_PACKAGE)
				print_result(dynamic_cast<VSCSendvaluePackage&>(
						*result).get_val_data(), 0);
		}
	}
	return 0;
}
