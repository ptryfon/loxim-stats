%{
#include <string>
#include <vector>
#include "TreeNode.h"
#include "Privilige.h"
  using namespace QParser;
  int yylex();
  int yyerror(char* s);
  TreeNode *d;
%}

%union {
  char* str;
  int num;
  double dbl;
  bool	bool_val;
  QParser::TreeNode* tree;
  QParser::QueryNode* qtree;
  QParser::FixPointNode* fxtree;
  QParser::CallProcNode* calltree;
  QParser::ProcedureNode* proctree;
  QParser::ViewNode* viewtree;
  QParser::Privilige *privilige;
  QParser::PriviligeListNode *priv_list;
  QParser::NameListNode *name_list;
}

%token	<num> INTEGER
%token	<dbl> DOUBLE
%token	<str> PARAMNAME NAME STRING SEMICOLON LEFTPAR RIGHTPAR SUM COUNT AVG MIN MAX DISTINCT DEREF REF BEGINTR END ABORT CREATE IF FI DO OD ELSE WHILE LINK FOREACH THEN FIX_OP FIXPOINT RETURN VERTICAL VALIDATE READ MODIFY DELETE PASSWD WITH REVOKE REMOVE TO USER FROM ON GRANT OPTION PROCEDURE LEFTPROCPAR RIGHTPROCPAR VIEW ONRETRIEVE ONUPDATE ONCREATE ONDELETE

%start statement

%right FIX_OP
%right SEMICOLON
%nonassoc RETURN
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

%type <bool_val> grant_option_opt
%type <tree> statement
%type <tree> validate_stmt
%type <tree> privilige_stmt
%type <tree> user_stmt
%type <priv_list> priv_defs
%type <name_list> name_defs
%type <privilige> privilige
%type <qtree> query
%type <proctree> procquery
%type <proctree> formparams
%type <viewtree> viewquery
%type <viewtree> viewrecdef
%type <viewtree> viewdef
%type <proctree> viewproc
%type <fxtree> querylist
%type <calltree> paramslist
%%

statement   : query semicolon_opt { d=$1; }
	    | BEGINTR semicolon_opt { d=new TransactNode(TransactNode::begin); }
	    | ABORT   semicolon_opt { d=new TransactNode(TransactNode::abort); }
	    | END     semicolon_opt { d=new TransactNode(TransactNode::end); }
	    | validate_stmt  semicolon_opt { d = $1; }
	    | privilige_stmt semicolon_opt { d = $1; }
	    | user_stmt	     semicolon_opt { d = $1; }
            ;

query	    : NAME { char *s = $1; $$ = new NameNode(s); delete s; }
	    | PARAMNAME { char *s = $1; $$ = new ParamNode(s); delete s; }
            | INTEGER { $$ = new IntNode($1); }
            | STRING { char *s = $1; $$ = new StringNode(s); delete s; }
	    | DOUBLE { $$ = new DoubleNode($1); }
            | LEFTPAR query RIGHTPAR { $$ = $2; }
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
	    | query REFEQUAL query { $$ = new AlgOpNode(new UnOpNode($1,UnOpNode::ref),new UnOpNode($3,UnOpNode::ref),AlgOpNode::eq);}
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
	    | query ASSIGN procquery { $$ = new AlgOpNode($1,$3,AlgOpNode::assign); }
	    | query ASSIGN viewquery { $$ = new AlgOpNode($1,$3,AlgOpNode::assign); }
	    | IF query THEN query ELSE query FI { $$ = new CondNode($2, $4, $6,CondNode::ifElsee); }
	    | IF query THEN query FI { $$ = new CondNode($2, $4, CondNode::iff); }
	    | WHILE query DO query OD { $$ = new CondNode($2, $4, CondNode::whilee); }
	    | FOREACH query DO query OD { $$ = new NonAlgOpNode($2, $4, NonAlgOpNode::forEach); }
	    | LINK STRING STRING INTEGER { $$ = new LinkNode($2, $3, $4); }
	    | query SEMICOLON query { $$ = new AlgOpNode ($1, $3, AlgOpNode::semicolon); }
	    | FIXPOINT LEFTPAR querylist RIGHTPAR { $$ = $3; }
	    | RETURN query {$$ = new ReturnNode ($2); }
	    | CREATE procquery {$$ = new RegisterProcNode ($2);}
	    | CREATE viewquery {$$ = new RegisterViewNode ($2);}
	    | NAME LEFTPAR RIGHTPAR {$$ = new CallProcNode ($1);}
	    | NAME LEFTPAR paramslist RIGHTPAR {$3->setName($1); $$ = $3; }
	    ;

querylist   : NAME FIX_OP query { $$ = new FixPointNode ($1, $3); }
	    | querylist NAME FIX_OP query {$$ = new FixPointNode ($2, $4, $1); }
	    ;

paramslist  : query { $$ = new CallProcNode ($1); }
	    | paramslist VERTICAL query {$$ = new CallProcNode ($3, $1); }
	    ;

procquery   : PROCEDURE NAME LEFTPAR formparams RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		char *n = $2; $4->addContent(n, $7); $$ = $4; delete n; }
	    | PROCEDURE NAME LEFTPAR RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		char *n = $2; $$ = new ProcedureNode(); $$->addContent(n, $6); delete n; }
            ;

formparams  : NAME {char *s = $1; $$ = new ProcedureNode (s); delete s;}
	    | formparams VERTICAL NAME {char *s = $3; $$ = new ProcedureNode (s, $1); delete s;}
	    ;

viewquery   : VIEW NAME LEFTPROCPAR viewrecdef RIGHTPROCPAR {
		char *n = $2; $4->setName(n); $$ = $4; delete n; }
            ;

viewrecdef  : viewdef		 { $$ = $1; }
            | viewrecdef viewdef { $1->addContent($2); }
            ;

viewdef     : viewproc	{ $$ = new ViewNode($1); }
            | viewquery	{ $$ = new ViewNode($1); }
            ;

viewproc    : PROCEDURE NAME LEFTPAR RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		$$ = new ProcedureNode(); char *n = $2; $$->addContent(n, $6); delete n; }
            | PROCEDURE ONUPDATE LEFTPAR NAME RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		$$ = new ProcedureNode(); char *p = $4; $$->addContent("on_update", $7); $$->addParam(p); delete p; }
            | PROCEDURE ONCREATE LEFTPAR NAME RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		$$ = new ProcedureNode(); char *p = $4; $$->addContent("on_create", $7); $$->addParam(p); delete p; }
            | PROCEDURE ONDELETE LEFTPAR RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		$$ = new ProcedureNode(); $$->addContent("on_delete", $6); }
            | PROCEDURE ONRETRIEVE LEFTPAR RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		$$ = new ProcedureNode(); $$->addContent("on_retrieve", $6); }
            ;

validate_stmt: VALIDATE NAME NAME  { $$ = new ValidationNode($2, $3); }
	    ;

privilige_stmt: GRANT priv_defs ON name_defs TO NAME grant_option_opt 	{ $$ = new GrantPrivNode($2, $4, $6, $7);  } 
	    |	REVOKE priv_defs ON name_defs FROM NAME			{ $$ = new RevokePrivNode($2, $4, $6); }
	    ;

grant_option_opt: /* empty */		{ $$ = false; }
	    |	  WITH GRANT OPTION	{ $$ = true;  }
	    ;
	    
privilige:	READ				{ $$ = new Privilige(Privilige::Read);   }
	    |	MODIFY				{ $$ = new Privilige(Privilige::Modify); }
	    |	CREATE				{ $$ = new Privilige(Privilige::Create); }
	    |	DELETE				{ $$ = new Privilige(Privilige::Delete); }
	    ;

priv_defs:	privilige			{ $$ = new PriviligeListNode($1);	}
	    |	priv_defs COMMA privilige	{ $$ = new PriviligeListNode($3, $1);	}
	    ;

name_defs:	NAME				{ $$ = new NameListNode($1); 	}
	    |	name_defs COMMA NAME		{ $$ = new NameListNode($3, $1);    }
	    ;
	    	    	    
user_stmt:	CREATE USER NAME PASSWD NAME	{ $$ = new CreateUserNode($3, $5); }
	    |	REMOVE USER NAME		{ $$ = new RemoveUserNode($3); 	   }
	    ;
	    
semicolon_opt:  /* empty */	{}
	    |	SEMICOLON	{}
	    ;
