%{
#include <string>
#include <vector>
#include "TreeNode.h"
  using namespace QParser;
  int yylex();
  int yyerror(char* s);
  TreeNode *d;
%}

%union {
  char* str;
  int num;
  double dbl;
  QParser::TreeNode* tree;
  QParser::QueryNode* qtree;
  QParser::FixPointNode* fxtree;
  QParser::StringNode* instree;
  QParser::ProcedureNode* proctree;
  QParser::FormPar* onepar;
  QParser::CallProcNode* calltree;
}

%token	<num> INTEGER
%token	<dbl> DOUBLE 
%token	<str> PARAMNAME NAME STRING SEMICOLON DOUBLESEMICOLON LEFTPAR RIGHTPAR SUM COUNT AVG MIN MAX DISTINCT DEREF REF BEGINTR END ABORT CREATE IF FI DO OD ELSE WHILE LINK FOREACH THEN FIX_OP FIXPOINT IN OUT RETURN VERTICAL
%token  <str> PROCEDURE LEFTPROCPAR RIGHTPROCPAR
%start statement

%right FIX_OP
%right SEMICOLON
%nonassoc DELETE CREATE INSERTINTO
%left COMMA 
%right EXISTS FOR_ALL
%left GROUP_AS AS
%right ASSIGN
%left UNION EXCEPT
%left INTERSECT
%right WHERE JOIN CLOSE_BY CLOSE_UNIQUE LEAVES_BY LEAVES_UNIQUE ORDER_BY 
%left OR
%left AND
%nonassoc NOT
%nonassoc EQUAL REFEQUAL NOT_EQUAL GREATER_THAN LESS_THAN GREATER_EQ LESS_EQ
%left PLUS MINUS
%left TIMES DIVIDE_BY
%nonassoc UMINUS
%right DOT

%type <tree> statement
%type <qtree> query
%type <fxtree> querylist
%type <instree> stringlist
%type <proctree> formparams
%type <onepar> formpar
%type <calltree> actparams
%%

statement   : query  { d=$1; }
	    | BEGINTR SEMICOLON { d=new TransactNode(TransactNode::begin); }
	    | BEGINTR { d=new TransactNode(TransactNode::begin); }
	    | ABORT   SEMICOLON { d=new TransactNode(TransactNode::abort); }
	    | ABORT { d=new TransactNode(TransactNode::abort); }
	    | END     SEMICOLON { d=new TransactNode(TransactNode::end); }
	    | END   { d=new TransactNode(TransactNode::end); }
            ;

query	    : NAME { char *s = $1; $$ = new NameNode(s); delete s; }
	    | PARAMNAME { char *s = $1; $$ = new ParamNode(s); delete s; }
            | INTEGER { $$ = new IntNode($1); }
            | STRING { char *s = $1; $$ = new StringNode(s); delete s; }
	    | DOUBLE { $$ = new DoubleNode($1); }
            | LEFTPAR query RIGHTPAR { $$ = $2; }
	    | query SEMICOLON {$$ = $1; }
            | query AS NAME { $$ = new NameAsNode($1,$3,false); }
	    | query GROUP_AS NAME { $$ = new NameAsNode($1,$3,true); }
            | COUNT LEFTPAR  query RIGHTPAR { $$ = new UnOpNode($3,UnOpNode::count); }
	    | SUM LEFTPAR  query RIGHTPAR { $$ = new UnOpNode($3,UnOpNode::sum); }
	    | AVG LEFTPAR  query RIGHTPAR { $$ = new UnOpNode($3,UnOpNode::avg); }
	    | MIN LEFTPAR  query RIGHTPAR { $$ = new UnOpNode($3,UnOpNode::min); }
	    | MAX LEFTPAR  query RIGHTPAR { $$ = new UnOpNode($3,UnOpNode::max); }
	    | DEREF LEFTPAR  query RIGHTPAR  { $$ = new UnOpNode($3,UnOpNode::deref); }
	    | REF LEFTPAR query RIGHTPAR { $$ = new UnOpNode($3, UnOpNode::ref); }
	    | DISTINCT LEFTPAR  query RIGHTPAR { $$ = new UnOpNode($3,UnOpNode::distinct); }
	    | MINUS query 				%prec UMINUS  { $$ = new UnOpNode($2,UnOpNode::unMinus); }
	    | NOT query { $$ = new UnOpNode($2,UnOpNode::boolNot); }
            | query UNION query { $$ = new AlgOpNode($1,$3,AlgOpNode::bagUnion); }
            | query INTERSECT query { $$ = new AlgOpNode($1,$3,AlgOpNode::bagIntersect); }
            | query EXCEPT query  { $$ = new AlgOpNode($1,$3,AlgOpNode::bagMinus); }
	    | query COMMA query { $$ = new AlgOpNode($1,$3,AlgOpNode::comma); } 
	    | query PLUS query { $$ = new AlgOpNode($1,$3,AlgOpNode::plus); }
            | query MINUS query { $$ = new AlgOpNode($1,$3,AlgOpNode::minus); }
	    | query TIMES query { $$ = new AlgOpNode($1,$3,AlgOpNode::times); }
	    | query DIVIDE_BY query { $$ = new AlgOpNode($1,$3,AlgOpNode::divide); }
	    | query EQUAL query { $$ = new AlgOpNode($1,$3,AlgOpNode::eq); }
	    | query REFEQUAL query {
	      $$ = new AlgOpNode(new UnOpNode($1,UnOpNode::ref),new UnOpNode($3,UnOpNode::ref),AlgOpNode::eq);}
	    | query NOT_EQUAL query { $$ = new AlgOpNode($1,$3,AlgOpNode::neq); }
	    | query GREATER_THAN query { $$ = new AlgOpNode($1,$3,AlgOpNode::gt); }
	    | query LESS_THAN query { $$ = new AlgOpNode($1,$3,AlgOpNode::lt); }
	    | query GREATER_EQ query { $$ = new AlgOpNode($1,$3,AlgOpNode::ge); }
	    | query LESS_EQ query { $$ = new AlgOpNode($1,$3,AlgOpNode::le); }
	    | query AND query { $$ = new AlgOpNode($1,$3,AlgOpNode::boolAnd); }
	    | query OR query { $$ = new AlgOpNode($1,$3,AlgOpNode::boolOr); }
	    | query DOT query { $$ = new NonAlgOpNode($1,$3,NonAlgOpNode::dot); }
	    | query JOIN query { $$ = new NonAlgOpNode($1,$3,NonAlgOpNode::join); }
	    | query WHERE query { $$ = new NonAlgOpNode($1,$3,NonAlgOpNode::where); }
	    | query CLOSE_BY      query { $$ = new NonAlgOpNode($1,$3,NonAlgOpNode::closeBy); }
	    | query CLOSE_UNIQUE  query { $$ = new NonAlgOpNode($1,$3,NonAlgOpNode::closeUniqueBy); }
	    | query LEAVES_BY	  query { $$ = new NonAlgOpNode($1,$3,NonAlgOpNode::leavesBy); }
	    | query LEAVES_UNIQUE query { $$ = new NonAlgOpNode($1,$3,NonAlgOpNode::leavesUniqueBy); }
	    | query ORDER_BY query { $$ = new NonAlgOpNode($1,$3,NonAlgOpNode::orderBy); }
	    | query EXISTS query { $$ = new NonAlgOpNode($1,$3,NonAlgOpNode::exists); }
	    | query FOR_ALL query { $$ = new NonAlgOpNode($1,$3,NonAlgOpNode::forAll); }
	    | CREATE query { $$ = new CreateNode($2); }
	    | query INSERTINTO query { $$ = new AlgOpNode($1,$3,AlgOpNode::insert); }
	    | DELETE query  { $$ = new UnOpNode($2,UnOpNode::deleteOp); }
	    | query ASSIGN query { $$ = new AlgOpNode($1,$3,AlgOpNode::assign); }
	    | IF query THEN query ELSE query FI { $$ = new CondNode($2, $4, $6,CondNode::ifElsee); }
	    | IF query THEN query FI { $$ = new CondNode($2, $4, CondNode::iff); }
	    | WHILE query DO query OD { $$ = new CondNode($2, $4, CondNode::whilee); }
	    | FOREACH query DO query OD { $$ = new NonAlgOpNode($2, $4, NonAlgOpNode::forEach); }
	    | LINK STRING STRING INTEGER { $$ = new LinkNode($2, $3, $4); }
	    | query SEMICOLON query { $$ = new AlgOpNode ($1, $3, AlgOpNode::semicolon); }
	    | FIXPOINT LEFTPAR querylist RIGHTPAR { $$ = $3; }
	    | PROCEDURE NAME LEFTPAR RIGHTPAR LEFTPROCPAR stringlist RIGHTPROCPAR {
		$$ = new ProcedureNode ($2, $6);}
	    | PROCEDURE NAME LEFTPAR formparams RIGHTPAR LEFTPROCPAR stringlist RIGHTPROCPAR {
		$$ = $4->addContent ($2, $7);}
	    | RETURN query {$$ = new ReturnNode ($2); }
	    | NAME LEFTPAR RIGHTPAR {$$ = new CallProcNode ($1);}
	    | NAME LEFTPAR actparams RIGHTPAR {$$ = $3->setName($1); }
	    ;
	    
querylist   : NAME FIX_OP query { $$ = new FixPointNode ($1, $3); }
	    | querylist NAME FIX_OP query {$$ = new FixPointNode ($2, $4, $1); }
	    ;
	    
stringlist  : STRING {char *s = $1; $$ = new StringNode(s); delete s;}
	    ;
	    
formparams  : formpar {$$ = new ProcedureNode ($1); }
	    | formparams SEMICOLON formpar {$$ = new ProcedureNode ($3, $1); }
	    ;
	
formpar	    : NAME { $$ = new FormPar ($1, "none"); }
	    | IN NAME { $$ = new FormPar ($2, "in"); }
	    | OUT NAME {$$ = new FormPar ($2, "out"); }
	    ;

actparams   : query { $$ = new CallProcNode ($1); }
	    | actparams VERTICAL query { $$ = new CallProcNode ($3, $1); }
	    ;
