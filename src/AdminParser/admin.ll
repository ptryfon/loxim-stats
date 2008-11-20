%option c++
%option prefix="Admin"
%option noyywrap

ws	[ \t\n]+

%{
    #include "admin.tab.hpp"
    #include "AdminLexer.h"
    #include <iostream>
    using namespace std;
%}

%%

{ws}		/*skip*/

#		{return AdminParser::AdminParser::token::HASH;}
;		{return AdminParser::AdminParser::token::SEMI;}
shutdown	{return AdminParser::AdminParser::token::KW_SHUTDOWN;}
<<EOF>>		{return AdminParser::AdminParser::token::ADMIN_EOF;}
.		{return AdminParser::AdminParser::token::ERROR;}

%%

int AdminParserlex(void *lval, AdminParser::AdminLexer *lexer)
{
    AdminParser::AdminParser::semantic_type *val = (AdminParser::AdminParser::semantic_type*)lval;
    return lexer->lex(val);
}

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
	return 0;
}
				   
				   