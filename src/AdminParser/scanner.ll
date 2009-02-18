%option c++
%option prefix="Admin"
%option noyywrap

ws	[ \t\n]+

%{
	#include <AdminParser/AdminParser.h>
	#include <AdminParser/AdminLexer.h>
	#include <iostream>
	#include <stdexcept>
	using namespace std;
%}

%%

{ws}		/*skip*/

#		{return HASH;}
;		{return SEMI;}
shutdown	{return KW_SHUTDOWN;}
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
				   
				   
