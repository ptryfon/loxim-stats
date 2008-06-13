#include <stdio.h>
#include <stdlib.h>

#include <getopt.h>
#include <string.h>
#include "LoximClient.h"
#include "ClientConsole.h"

#include "ReadlineReader.h"
#include "ConsoleAuthenticator.h"
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 2000

using namespace protocol;

void add_sig_handler(int sig, void (*handler)(int))
{
	struct sigaction s_action;
	s_action.sa_handler = handler;
	s_action.sa_flags = 0;
	s_action.sa_restorer = 0;
	if (sigemptyset(&s_action.sa_mask)){
		fprintf(stderr, "Error while initializing empty signal set\n");
		exit(1);
	};
	if (sigaction(sig, &s_action, NULL)){
		fprintf(stderr, "Error while adding signal handler (%d)\n", sig);
		exit(1);
	};

}

void sig_handler(int a)
{
	printf("In order to quit, type \"quit\"\n");	
}

void print_usage(FILE *stream, char* program_name, int exit_code)
{
	fprintf(stream, "U�ycie %s [opcje]\n", program_name);
	fprintf(stream, 
		"             --help          Wy�wietla te informacje\n"
		" -h [adres]  --host [adres]  Ustawia adres (nazw�) serwera do kt�rego si� ��czymy (domy�lnie: %s)\n"
		" -p [port]   --port [port]   Ustawia port do kt�rego si� ��czymy (domy�lnie: %d)\n"
		" -l [login]  --login [login] Ustawia login (jezeli brak, to klient zapyta)\n"
		" -P [passwd] --password [passwd] Ustawia haslo (jezeli brak, to klient zapyta)\n"
		" -m [tryb]   --mode [tryb]   Poczatkowy wczytywania polecen (dot|slash) (domyslnie: slash)\n"
		" -f [plik]   --file [plik]   Skrypt z zapytaniami\n",
		DEFAULT_HOST,DEFAULT_PORT
	);
	exit(exit_code);
};

bool is_number(char *n)
{
	for (;*n; n++)
		if (!isdigit(*n))
			return false;
	return true;
}


/**
 * Metoda parsuje opcje i przekazuje je do klasy LoximClient 
 */
int main(int argc, char* argv[])
{
	printf("Welcome to loxim client.\n\n");
	/*
	ReadlineReader *r=new ReadlineReader();
	while(true)
	{
		printf("otrzymano: %s\n\n",r->readStatement());
	};*/
	
	/*Dost�pne kr�tkie opcje*/
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
	char *login = 0, *password = 0;
	FILE *file = 0;
	char* hostname=NULL;
	unsigned int port=DEFAULT_PORT;
	int mode = CC_DOT;
	int next_option;
	do {
		next_option=getopt_long(argc,argv,short_opt, long_options,NULL);
		switch (next_option)
		{
			case 'h':
				hostname=strdup(optarg);
				break;
				
			case 'p':
				if (!is_number(optarg)){
					printf("Invalid port number\n");
					exit(-1);
				} else {
					port= atoi(optarg);
				}
				port=atoi(optarg);
				break;
				
			case 'H':
				print_usage(stdout,argv[0],0);
				break;

			case 'l':
				login = strdup(optarg);
				break;

			case 'P':
				password = strdup(optarg);
				break;
			case 'f':
				file = fopen(optarg, "r");
				if (!file){
					printf("Cannot open file %s\nError %d: %s\n", optarg, errno, strerror(errno));
					exit(-1);
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
				print_usage(stdout, argv[0], -1);
			case -1:
				break;
				
			default:
				print_usage(stdout,argv[0],-1);
				break;
		}
	}while (next_option != -1);
	
	LoximClient::LoximClient *loximClient=new LoximClient::LoximClient(hostname?hostname:DEFAULT_HOST, port);
	add_sig_handler(SIGINT, sig_handler);
	int res=loximClient->connect();
	if (res<0)
	{
		fprintf(stderr, "Cannot connect to: %s:%d\n",hostname?hostname:DEFAULT_HOST, port);
	}
	
	if (hostname)
		free(hostname);
	
	if (res>0)
	{	
		printf("Connected :)\n");
		if (isatty(1))
			printf("Type $help to see the available commands\n");
		LoximClient::ConsoleAuthenticator *auth;
		if (login && password)
			auth = new LoximClient::ConsoleAuthenticator(login, password);
		else
			if (login)
				auth = new LoximClient::ConsoleAuthenticator(login);
			else
				auth = new LoximClient::ConsoleAuthenticator();
		auth->read();
		res=loximClient->authorize(auth);
		delete auth;
		if (res>0)
		{
			printf("\nAuthorized :)\n");

			LoximClient::ClientConsole *cc;
			if (file)
				cc = new LoximClient::ClientConsole(mode, file);
			else
				cc = new LoximClient::ClientConsole(mode);
			res=loximClient->run(cc);
			delete cc;
		}else
		{
			fprintf(stderr, "Cannot authorize.\n");	
		}
	}
	
	delete loximClient;

	return res;
}
