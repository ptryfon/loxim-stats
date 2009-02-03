%defines
%pure-parser
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

