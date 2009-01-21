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

#include <Client/Client.h>
#include <Client/ClientConsole.h>
#include <Client/ConsoleAuthenticator.h>

#define DEFAULT_HOSTNAME "localhost"
#define DEFAULT_PORT 2000

using namespace Protocol;
using namespace Client;

auto_ptr< ::Client::Client> client;

void sig_handler(int a)
{
	client->aborter.trigger();
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
					cout << "Error " << errno << ": (" << strerror(errno) << ")" << endl;
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
	
	
	client= auto_ptr< ::Client::Client>(new ::Client::Client(get_host_ip(hostname), htons(port)));
	
	cout << "Connected." << endl;

	if (isatty(0))
		cout << "Type $help to see the available commands" << endl;
	{
		auto_ptr<ConsoleAuthenticator> auth;
		if (login.size() && password.size())
			auth = auto_ptr<ConsoleAuthenticator>(new ConsoleAuthenticator(login, password));
		else
			if (login.size())
				auth = auto_ptr<ConsoleAuthenticator>(new ConsoleAuthenticator(login));
			else
				auth = auto_ptr<ConsoleAuthenticator>(new ConsoleAuthenticator());
		auth->read();
		client->authorize(*auth);
	}
	cout << "Authorized." << endl;

	auto_ptr<ClientConsole> cc;
	if (file.get())
		cc = auto_ptr<ClientConsole>(new ClientConsole(mode, file));
	else
		cc = auto_ptr<ClientConsole>(new ClientConsole(mode));
	client->run(*cc);
	return 0;
}
