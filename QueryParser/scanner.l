%x comment
%option yylineno

%x yycomment
%x yystring
%option c++
%option noyywrap

%%
"("			return LEFTPAR;
")"			return RIGHTPAR;
";"			return SEMICOLON;
"sum"			return SUM;
"count"			return COUNT;
"avg"			return AVG;
"min"			return MIN;
"max"			return MAX;
"distinct"		return DISTINCT;
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
"insert"		return INSERT;
"into"			return INTO;
"delete"		return DELETE;
":="			return ASSIGN;
"deref"			return DEREF;
"begin"			return BEGIN;
"end"			return END;
"abort"			return ABORT;


[_a-zA-Z][_a-zA-Z0-9]* 	yylval.str = new char[strlen(yytext) + 1]; yylval.str = strcpy(yylval.str,yytext); return NAME;
[0-9]+ 			yylval.licz = atoi(yytext); return INTEGER;
[0-9]+"."[0-9]+		yylval.dbl = atof(yytext); return DOUBLE;

"{"	BEGIN(comment);
<comment>"}" BEGIN(INITIAL);
<comment>.|\n

\"[^\"]*\"		yylval.str = new char[strlen(yytext)]; yylval.str = strncpy(yylval.str, &yytext[1], strlen(yytext)-2); yylval.str[strlen(yytext)-2]=0; return STRING;

[ \t\r\n]+		/* ignore whitespace */;