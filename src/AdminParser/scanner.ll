%option c++
%option prefix="Admin"
%option noyywrap

ws	[ \t\n]+

%{
	#include <AdminParser/AdminParser.h>
	#include <AdminParser/AdminLexer.h>
	#include <iostream>
	#include <stdexcept>
	#include <cstdio>
	using namespace std;
%}

%%

{ws}		/*skip*/

#		{return HASH;}
;		{return SEMI;}
[0-9]+		{yylval->number = atoi(yytext); return NUMBER;}
shutdown	{return KW_SHUTDOWN;}
start		{return KW_START;}
stop		{return KW_STOP;}
clear		{return KW_CLEAR;}
stats		{return KW_STATS;}
dump		{return KW_DUMP;}
all		{return KW_ALL;}
store		{return KW_STORE;}
session		{return KW_SESSION;}
transaction	{return KW_TRANSACTION;}
queries		{return KW_QUERIES;}
config		{return KW_CONFIG;}
<<EOF>>		{return ADMIN_EOF;}
.		{return ERROR;}

%%


AdminParser::AdminLexer::AdminLexer(std::istream* in, std::ostream* out) : AdminFlexLexer(in, out)
{
}
		     
AdminParser::AdminLexer::~AdminLexer()
{
}
		     
#ifdef yylex
#undef yylex
#endif
			   
int AdminFlexLexer::yylex()
{
	//should never be called
	throw runtime_error("admin statements lexer internal error");
}
				   
				   
