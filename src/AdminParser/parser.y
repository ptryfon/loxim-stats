%defines
%{
	#include <SystemStats/StatsTypes.h>
%}
%pure-parser
%union{
	AdminTreeNode *node;
	SystemStatsLib::StatsScope stats_opt;
	int number;
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
%token KW_SET
%token KW_INTERVAL
%token <number> NUMBER

%%


%type <node> cmd;
%type <stats_opt> opt_stats;
%type <number> number;
%start cmd;

cmd:  
	  HASH KW_SHUTDOWN opt_semi ADMIN_EOF { *result = new ShutdownTreeNode(); YYACCEPT; }
	| HASH KW_START opt_stats KW_STATS ADMIN_EOF { *result = new StatsControlNode(START_GLOBAL, $3); YYACCEPT; }
	| HASH KW_STOP opt_stats KW_STATS ADMIN_EOF { *result = new StatsControlNode(STOP_GLOBAL, $3); YYACCEPT; }
	| HASH KW_START opt_stats KW_STATS KW_DUMP ADMIN_EOF { *result = new StatsControlNode(START_DUMP, $3); YYACCEPT; }
	| HASH KW_STOP opt_stats KW_STATS KW_DUMP ADMIN_EOF { *result = new StatsControlNode(STOP_DUMP, $3); YYACCEPT; }
	| HASH KW_CLEAR opt_stats KW_STATS ADMIN_EOF { *result = new StatsControlNode(CLEAR_GLOBAL, $3); YYACCEPT; }
	| HASH KW_SET opt_stats KW_STATS KW_DUMP KW_INTERVAL number ADMIN_EOF { *result = new StatsControlNode(SET_DUMP_INTERVAL, $3, $7); YYACCEPT; }
   	| error { YYABORT; }; 
opt_semi: 
    | SEMI;
opt_stats:
	 {$$ = SystemStatsLib::ALL;}
	| KW_ALL { $$ = SystemStatsLib::ALL; }
	| KW_STORE { $$ = SystemStatsLib::STORE; }
	| KW_SESSION { $$ = SystemStatsLib::SESSION; }
	| KW_TRANSACTION { $$ = SystemStatsLib::TRANSACTION; }
	| KW_QUERIES { $$ = SystemStatsLib::QUERIES; }
	| KW_CONFIG { $$ = SystemStatsLib::CONFIG; }
number:
	NUMBER { $$ = $1; }
	 
%%

