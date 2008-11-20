%x comment
%option yylineno
%option prefix="Query"
%x yycomment
%x yystring
%option c++
%option noyywrap
%{
#include <iostream>
#include <QueryParser/TreeNode.h>
#include <QueryParser/parser.h>
#include <QueryParser/QueryLexer.h>
#include <string>
#  ifndef YYERRCODE
#    define YYERRCODE 256
#  endif  
using namespace std;

%}
%%
"("			return QParser::QueryParserGen::token::LEFTPAR;
")"			return QParser::QueryParserGen::token::RIGHTPAR;
";"			return QParser::QueryParserGen::token::SEMICOLON;
"create or update" 	return QParser::QueryParserGen::token::CREATE_OR_UPDATE;
"sum"			return QParser::QueryParserGen::token::SUM;
"count"			return QParser::QueryParserGen::token::COUNT;
"avg"			return QParser::QueryParserGen::token::AVG;
"min"			return QParser::QueryParserGen::token::MIN;
"max"			return QParser::QueryParserGen::token::MAX;
"distinct"		return QParser::QueryParserGen::token::DISTINCT;
"includes"		return QParser::QueryParserGen::token::INCLUDES;
"excludes"		return QParser::QueryParserGen::token::EXCLUDES;
"group as"		return QParser::QueryParserGen::token::GROUP_AS;
"as"			return QParser::QueryParserGen::token::AS;
"union"			return QParser::QueryParserGen::token::UNION;
"intersect"		return QParser::QueryParserGen::token::INTERSECT;
"exists"		return QParser::QueryParserGen::token::EXISTS;
"for all"		return QParser::QueryParserGen::token::FOR_ALL;
"where"			return QParser::QueryParserGen::token::WHERE;
"join"			return QParser::QueryParserGen::token::JOIN;
"close by"		return QParser::QueryParserGen::token::CLOSE_BY;
"close unique by"	return QParser::QueryParserGen::token::CLOSE_UNIQUE;
"leaves by"		return QParser::QueryParserGen::token::LEAVES_BY;
"leaves unique by"	return QParser::QueryParserGen::token::LEAVES_UNIQUE;
"order by"		return QParser::QueryParserGen::token::ORDER_BY;
"or"			return QParser::QueryParserGen::token::OR;
"and"			return QParser::QueryParserGen::token::AND;
"not"			return QParser::QueryParserGen::token::NOT;
"="			return QParser::QueryParserGen::token::EQUAL;
"=="			return QParser::QueryParserGen::token::REFEQUAL;
"!="			return QParser::QueryParserGen::token::NOT_EQUAL;
">"			return QParser::QueryParserGen::token::GREATER_THAN;
"<"			return QParser::QueryParserGen::token::LESS_THAN;
">="			return QParser::QueryParserGen::token::GREATER_EQ;
"<="			return QParser::QueryParserGen::token::LESS_EQ;
"+"			return QParser::QueryParserGen::token::PLUS;
"-"			return QParser::QueryParserGen::token::MINUS;
"*"			return QParser::QueryParserGen::token::TIMES;
"/"			return QParser::QueryParserGen::token::DIVIDE_BY;
"minus"			return QParser::QueryParserGen::token::EXCEPT;
"."			return QParser::QueryParserGen::token::DOT;
","			return QParser::QueryParserGen::token::COMMA;
":"			return QParser::QueryParserGen::token::COLON;
":<"			return QParser::QueryParserGen::token::INSERTINTO;
"delete"		return QParser::QueryParserGen::token::DELETE;
":="			return QParser::QueryParserGen::token::ASSIGN;
"deref"			return QParser::QueryParserGen::token::DEREF;
"ref"			return QParser::QueryParserGen::token::REF;
"nameof"		return QParser::QueryParserGen::token::NAMEOF;
"reloadScheme"		return QParser::QueryParserGen::token::RELOADSCHEME;
"begin"			return QParser::QueryParserGen::token::BEGINTR;
"end"			return QParser::QueryParserGen::token::END;
"abort"			return QParser::QueryParserGen::token::ABORT; 
"create"		return QParser::QueryParserGen::token::CREATE;
"update"		return QParser::QueryParserGen::token::UPDATE;
"interface"		return QParser::QueryParserGen::token::INTERFACE;
"for each"		return QParser::QueryParserGen::token::FOREACH;
"do"			return QParser::QueryParserGen::token::DO;
"od"			return QParser::QueryParserGen::token::OD;
"if"			return QParser::QueryParserGen::token::IF;
"then"			return QParser::QueryParserGen::token::THEN;
"else"			return QParser::QueryParserGen::token::ELSE;
"fi"			return QParser::QueryParserGen::token::FI;
"while"			return QParser::QueryParserGen::token::WHILE;
"link"			return QParser::QueryParserGen::token::LINK;
"fixpoint"		return QParser::QueryParserGen::token::FIXPOINT;
":-"			return QParser::QueryParserGen::token::FIX_OP;
"return"		return QParser::QueryParserGen::token::RETURN;
"break"			return QParser::QueryParserGen::token::BREAK;
"procedure"		return QParser::QueryParserGen::token::PROCEDURE;
"class"         	return QParser::QueryParserGen::token::CLASS;
"extends"       	return QParser::QueryParserGen::token::EXTENDS;
"instance"      	return QParser::QueryParserGen::token::INSTANCE;
"instanceof"    	return QParser::QueryParserGen::token::INSTANCEOF;
"cast"			return QParser::QueryParserGen::token::CAST;
"static"		return QParser::QueryParserGen::token::STATIC;
"{"			return QParser::QueryParserGen::token::LEFTPROCPAR;
"}"			return QParser::QueryParserGen::token::RIGHTPROCPAR;
"["			return QParser::QueryParserGen::token::LEFTSQUAREPAR;
"]"			return QParser::QueryParserGen::token::RIGHTSQUAREPAR;
"view"			return QParser::QueryParserGen::token::VIEW;
"on_update"		return QParser::QueryParserGen::token::ONUPDATE;
"on_retrieve"		return QParser::QueryParserGen::token::ONRETRIEVE;
"on_delete"		return QParser::QueryParserGen::token::ONDELETE;
"on_create"		return QParser::QueryParserGen::token::ONCREATE;
"on_insert"		return QParser::QueryParserGen::token::ONINSERT;
"on_navigate"		return QParser::QueryParserGen::token::ONNAVIGATE;
"on_virtualize"		return QParser::QueryParserGen::token::ONVITRUALIZE;
"on_store"		return QParser::QueryParserGen::token::ONSTORE;
"validate"		return QParser::QueryParserGen::token::VALIDATE;
"grant"			return QParser::QueryParserGen::token::GRANT;
"revoke"		return QParser::QueryParserGen::token::REVOKE;
"to"			return QParser::QueryParserGen::token::TO;
"from"			return QParser::QueryParserGen::token::FROM;
"on"			return QParser::QueryParserGen::token::ON;
"with"			return QParser::QueryParserGen::token::WITH;
"option"		return QParser::QueryParserGen::token::OPTION;
"read"			return QParser::QueryParserGen::token::READ;
"modify"		return QParser::QueryParserGen::token::MODIFY;
"user"			return QParser::QueryParserGen::token::USER;
"passwd"		return QParser::QueryParserGen::token::PASSWD;
"remove"		return QParser::QueryParserGen::token::REMOVE;
"index"			return QParser::QueryParserGen::token::INDEX;
"virtual objects"	return QParser::QueryParserGen::token::VIRTUAL;
"virtualize as"		return QParser::QueryParserGen::token::VIRTUALIZE_AS;
"shows"			return QParser::QueryParserGen::token::SHOWS;
"associate"		return QParser::QueryParserGen::token::ASSOCIATE;
"typedef"		return QParser::QueryParserGen::token::TYPEDEF;
"string"		return QParser::QueryParserGen::token::STRING_SIG; /* uzywane tez przez indeksy */
"double"		return QParser::QueryParserGen::token::DOUBLE_SIG; /* uzywane tez przez indeksy */
"integer"		return QParser::QueryParserGen::token::INTEGER_SIG;
"boolean"		return QParser::QueryParserGen::token::BOOLEAN_SIG;/* TO BE OR NOT TO BE... */
"tcOFF"			return QParser::QueryParserGen::token::TCOFF;
"tcON"			return QParser::QueryParserGen::token::TCON;
"int"			return QParser::QueryParserGen::token::INT_SIG;
"(|"			return QParser::QueryParserGen::token::LEFT_EXCLUSIVE;
"|)"			return QParser::QueryParserGen::token::RIGHT_EXCLUSIVE;
"<|"			return QParser::QueryParserGen::token::LEFT_INCLUSIVE;
"|>"			return QParser::QueryParserGen::token::RIGHT_INCLUSIVE;
"|"			return QParser::QueryParserGen::token::INDEXPAR;
"throw"			return QParser::QueryParserGen::token::THROW;
"object name is"	return QParser::QueryParserGen::token::OBJECT_NAME_IS;

([_a-zA-Z][_a-zA-Z0-9]*)*::[_a-zA-Z][_a-zA-Z0-9]* yylval->str = new char[strlen(yytext) + 1]; yylval->str = strcpy(yylval->str,yytext); return QParser::QueryParserGen::token::EXTNAME;
[_a-zA-Z][_a-zA-Z0-9]* 	yylval->str = new char[strlen(yytext) + 1]; yylval->str = strcpy(yylval->str,yytext); return QParser::QueryParserGen::token::NAME;
$[_a-zA-Z][_a-zA-Z0-9]* yylval->str = new char[strlen(yytext) + 1]; yylval->str = strcpy(yylval->str,yytext); return QParser::QueryParserGen::token::PARAMNAME;
%[_a-zA-Z][_a-zA-Z0-9]* 	yylval->str = new char[strlen(yytext) + 1]; yylval->str = strcpy(yylval->str,yytext); return QParser::QueryParserGen::token::SYSTEMVIEWNAME;
[0-1]".."[0-1"*"] yylval->str = new char[strlen(yytext) + 1]; yylval->str = strcpy(yylval->str,yytext); return QParser::QueryParserGen::token::CARDCONST;
[0-9]+ 			yylval->num = atoi(yytext); return QParser::QueryParserGen::token::INTEGER;
[0-9]+"."[0-9]+		yylval->dbl = atof(yytext); return QParser::QueryParserGen::token::DOUBLE;

"/*"	BEGIN(comment);
<comment>"*/" BEGIN(INITIAL);
<comment>.|\n

\"[^\"]*\"		yylval->str = new char[strlen(yytext)]; yylval->str = strncpy(yylval->str, &yytext[1], strlen(yytext)-2); yylval->str[strlen(yytext)-2]=0; return QParser::QueryParserGen::token::STRING;

[ \t\r\n]+		/* ignore whitespace */;

%%


int QueryFlexLexer::yylex()
{
    //shouldn't be called
    return 0;
}

int QParserlex(void *lval, QParser::QueryLexer *lexer)
{
    QParser::QueryParserGen::semantic_type *val = (QParser::QueryParserGen::semantic_type*)lval;
    return lexer->lex(val);
}

QParser::QueryLexer::QueryLexer(std::istream *in, std::ostream *out) : QueryFlexLexer(in, out)
{
}

QParser::QueryLexer::~QueryLexer()
{
}
