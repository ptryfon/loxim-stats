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
#include <QueryParser/ParserWrapper.h>
#include <QueryParser/QueryLexer.h>
#include <string>
#include <stdexcept>
#  ifndef YYERRCODE
#    define YYERRCODE 256
#  endif  
using namespace std;
using namespace QParser;

%}
%%
"("			return LEFTPAR;
")"			return RIGHTPAR;
";"			return SEMICOLON;
"create or update" 	return CREATE_OR_UPDATE;
"sum"			return SUM;
"count"			return COUNT;
"avg"			return AVG;
"min"			return MIN;
"max"			return MAX;
"distinct"		return DISTINCT;
"includes"		return INCLUDES;
"excludes"		return EXCLUDES;
"group as"		return GROUP_AS;
"as"			return AS;
"union"			return UNION;
"intersect"		return INTERSECT;
"exists"		return EXISTS;
"for all"		return FOR_ALL;
"where"			return WHERE;
"join"			return JOIN;
"close by"		return CLOSE_BY;
"close unique by"	return CLOSE_UNIQUE;
"leaves by"		return LEAVES_BY;
"leaves unique by"	return LEAVES_UNIQUE;
"order by"		return ORDER_BY;
"or"			return OR;
"and"			return AND;
"not"			return NOT;
"="			return EQUAL;
"=="			return REFEQUAL;
"!="			return NOT_EQUAL;
">"			return GREATER_THAN;
"<"			return LESS_THAN;
">="			return GREATER_EQ;
"<="			return LESS_EQ;
"+"			return PLUS;
"-"			return MINUS;
"*"			return TIMES;
"/"			return DIVIDE_BY;
"minus"			return EXCEPT;
"."			return DOT;
","			return COMMA;
":"			return COLON;
":<"			return INSERTINTO;
"delete"		return DELETE;
":="			return ASSIGN;
"deref"			return DEREF;
"ref"			return REF;
"nameof"		return NAMEOF;
"reloadScheme"		return RELOADSCHEME;
"begin"			return BEGINTR;
"end"			return END;
"abort"			return ABORT; 
"create"		return CREATE;
"update"		return UPDATE;
"interface"		return INTERFACE;
"for each"		return FOREACH;
"do"			return DO;
"od"			return OD;
"if"			return IF;
"then"			return THEN;
"else"			return ELSE;
"fi"			return FI;
"while"			return WHILE;
"link"			return LINK;
"fixpoint"		return FIXPOINT;
":-"			return FIX_OP;
"return"		return RETURN;
"break"			return BREAK;
"procedure"		return PROCEDURE;
"class"         	return CLASS;
"extends"       	return EXTENDS;
"instance"      	return INSTANCE;
"instanceof"    	return INSTANCEOF;
"cast"			return CAST;
"static"		return STATIC;
"{"			return LEFTPROCPAR;
"}"			return RIGHTPROCPAR;
"["			return LEFTSQUAREPAR;
"]"			return RIGHTSQUAREPAR;
"view"			return VIEW;
"on_update"		return ONUPDATE;
"on_retrieve"		return ONRETRIEVE;
"on_delete"		return ONDELETE;
"on_create"		return ONCREATE;
"on_insert"		return ONINSERT;
"on_navigate"		return ONNAVIGATE;
"on_virtualize"		return ONVITRUALIZE;
"on_store"		return ONSTORE;
"validate"		return VALIDATE;
"grant"			return GRANT;
"revoke"		return REVOKE;
"to"			return TO;
"from"			return FROM;
"on"			return ON;
"with"			return WITH;
"option"		return OPTION;
"read"			return READ;
"modify"		return MODIFY;
"user"			return USER;
"passwd"		return PASSWD;
"remove"		return REMOVE;
"index"			return INDEX;
"virtual objects"	return VIRTUAL;
"virtualize as"		return VIRTUALIZE_AS;
"shows"			return SHOWS;
"associate"		return ASSOCIATE;
"typedef"		return TYPEDEF;
"string"		return STRING_SIG; /* uzywane tez przez indeksy */
"double"		return DOUBLE_SIG; /* uzywane tez przez indeksy */
"integer"		return INTEGER_SIG;
"boolean"		return BOOLEAN_SIG;/* TO BE OR NOT TO BE... */
"tcOFF"			return TCOFF;
"tcON"			return TCON;
"int"			return INT_SIG;
"(|"			return LEFT_EXCLUSIVE;
"|)"			return RIGHT_EXCLUSIVE;
"<|"			return LEFT_INCLUSIVE;
"|>"			return RIGHT_INCLUSIVE;
"|"			return INDEXPAR;
"throw"			return THROW;
"object name is"	return OBJECT_NAME_IS;
"schema"		return SCHEMA;
"export"		return EXPORT;
"import"		return IMPORT;

([_a-zA-Z][_a-zA-Z0-9]*)*::[_a-zA-Z][_a-zA-Z0-9]* yylval->str = new char[strlen(yytext) + 1]; yylval->str = strcpy(yylval->str,yytext); return EXTNAME;
[_a-zA-Z][_a-zA-Z0-9]* 	yylval->str = new char[strlen(yytext) + 1]; yylval->str = strcpy(yylval->str,yytext); return NAME;
$[_a-zA-Z][_a-zA-Z0-9]* yylval->str = new char[strlen(yytext) + 1]; yylval->str = strcpy(yylval->str,yytext); return PARAMNAME;
%[_a-zA-Z][_a-zA-Z0-9]* 	yylval->str = new char[strlen(yytext) + 1]; yylval->str = strcpy(yylval->str,yytext); return SYSTEMVIEWNAME;
[0-1]".."[0-1"*"] yylval->str = new char[strlen(yytext) + 1]; yylval->str = strcpy(yylval->str,yytext); return CARDCONST;
[0-9]+ 			yylval->num = atoi(yytext); return INTEGER;
[0-9]+"."[0-9]+		yylval->dbl = atof(yytext); return DOUBLE;
refid\([0-9]+\)		yylval->num = atoi(yytext+6); return REFID;

"/*"	BEGIN(comment);
<comment>"*/" BEGIN(INITIAL);
<comment>.|\n

\"[^\"]*\"		yylval->str = new char[strlen(yytext)]; yylval->str = strncpy(yylval->str, &yytext[1], strlen(yytext)-2); yylval->str[strlen(yytext)-2]=0; return STRING;

[ \t\r\n]+		/* ignore whitespace */;

%%


int QueryFlexLexer::yylex()
{
    //shouldn't be called
    throw runtime_error("internal parser error");
}


QParser::QueryLexer::QueryLexer(std::istream *in, std::ostream *out) : QueryFlexLexer(in, out)
{
}

QParser::QueryLexer::~QueryLexer()
{
}
