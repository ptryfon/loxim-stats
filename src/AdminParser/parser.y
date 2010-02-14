%defines
%pure-parser
%union{
    AdminTreeNode *node;
	int stats_opt;
}

%lex-param{ AdminLexer *lexer }
%parse-param{ AdminLexer *lexer }
%parse-param{ AdminExecutableTreeNode **result }

%token KW_SHUTDOWN
%token SEMI
%token ERROR
%token ADMIN_EOF
%token HASH
%token KW_START
%token KW_STOP
%token KW_CLEAR
%token KW_STATS
%token KW_DUMP
%token KW_ALL
%token KW_STORE
%token KW_SESSION
%token KW_TRANSACTION
%token KW_QUERIES
%token KW_CONFIG

%%


%type <node> cmd;
%type <stats_opt> opt_stats;
%start cmd;

cmd:  
	  HASH KW_SHUTDOWN opt_semi ADMIN_EOF { *result = new ShutdownTreeNode(); YYACCEPT; }
	| HASH KW_START opt_stats KW_STATS { *result = new StatsControlNode(START_GLOBAL, $3); YYACCEPT; }
	| HASH KW_STOP opt_stats KW_STATS { *result = new StatsControlNode(STOP_GLOBAL, $3); YYACCEPT; }
	| HASH KW_START opt_stats KW_STATS KW_DUMP { *result = new StatsControlNode(START_DUMP, $3); YYACCEPT; }
	| HASH KW_STOP opt_stats KW_STATS KW_DUMP { *result = new StatsControlNode(STOP_DUMP, $3); YYACCEPT; }
	| HASH KW_CLEAR opt_stats KW_STATS { *result = new StatsControlNode(CLEAR_GLOBAL, $3); YYACCEPT; }
    | error { YYABORT; }; 
opt_semi: 
    | SEMI;
opt_stats:
	 {$$ = KW_ALL;}
	| KW_ALL { $$ = KW_ALL; }
	| KW_STORE { $$ = KW_STORE; }
	| KW_SESSION { $$ = KW_SESSION; }
	| KW_TRANSACTION { $$ = KW_TRANSACTION; }
	| KW_QUERIES { $$ = KW_QUERIES; }
	| KW_CONFIG { $$ = KW_CONFIG; }
	 
%%

