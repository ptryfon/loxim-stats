%skeleton "lalr1.cc"
%defines
%define "parser_class_name" "AdminParser"
%name-prefix="AdminParser"
%{ 

#include "AdminExecutableTreeNode.h"
#include "ShutdownTreeNode.h"
namespace AdminParser{
    class AdminLexer;
}
int AdminParserlex(void *lval, AdminParser::AdminLexer *lexer);

%}
%union{
    AdminTreeNode *node;
}

%lex-param{ AdminLexer *lexer }
%parse-param{ AdminLexer *lexer }
%parse-param{ AdminExecutableTreeNode **result }

%token KW_SHUTDOWN
%token SEMI
%token ERROR
%token ADMIN_EOF
%token HASH

%%


%type <node> cmd;
%start cmd;

cmd: HASH KW_SHUTDOWN opt_semi ADMIN_EOF { *result = new ShutdownTreeNode(); YYACCEPT; }
    | error { YYABORT; }; 
opt_semi: 
    | SEMI;
     

%%

void AdminParser::AdminParser::error(const AdminParser::AdminParser::location_type& l, const std::string& m)
{
    
}
