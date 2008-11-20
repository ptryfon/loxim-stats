#include <LoximClient/ReadlineReader.h>
#include <LoximClient/StatementSimpleParser.h>

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>

using namespace protocol;
using namespace LoximClient;

ReadlineReader::ReadlineReader()
{
	rl_bind_key ('\t', rl_insert);
}

char* ReadlineReader::readStatement()
{
	char *stmt=NULL;
	char promptc= '=';
	
	char prompt[10]="loximx# ";
	
	StatementSimpleParser *parser=new StatementSimpleParser();
	do
	{
		prompt[5]=promptc;
		char* res=readline(prompt);
		if (res!=NULL)
		{
			parser->append(res);
			parser->append("\n");  //TODO: Wydajno¶ciowy ból II 
			free(res);
		}
		else
		{
			free(res);
			delete parser;
			return NULL;
		};
		stmt=parser->getStatement(&promptc);
		if (promptc==0)
			promptc='-';
	}while(stmt==NULL);
	add_history(stmt);
	return stmt;
}

