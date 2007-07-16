%{
#include <string>
#include <vector>
#include "TreeNode.h"
#include "IndexNode.h"
#include "Privilige.h"
  using namespace QParser;
  using namespace Indexes;
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
  QParser::VectorNode* vectree;
  QParser::ProcedureNode* proctree;
  QParser::ViewNode* viewtree;
  QParser::ClassNode* classtree;
  QParser::Privilige *privilige;
  QParser::PriviligeListNode *priv_list;
  QParser::NameListNode *name_list;
  QParser::InterfaceAttribute *attribute;
  QParser::InterfaceMethodParam* method_param;
  QParser::InterfaceAttributeListNode *attributes;
  QParser::InterfaceMethod* method;
  QParser::InterfaceMethodListNode *methods;
  QParser::InterfaceMethodParamListNode* method_params;
  QParser::InterfaceStruct* iStruct;
}

%token	<num> INTEGER
%token	<dbl> DOUBLE
%token	<str> PARAMNAME NAME STRING SEMICOLON LEFTPAR RIGHTPAR SUM COUNT AVG MIN MAX DISTINCT DEREF REF NAMEOF RELOADSCHEME BEGINTR END ABORT CREATE IF FI DO OD ELSE WHILE LINK FOREACH THEN FIX_OP FIXPOINT RETURN BREAK VALIDATE READ MODIFY DELETE PASSWD WITH REVOKE REMOVE TO USER FROM ON GRANT OPTION PROCEDURE CLASS EXTENDS INSTANCE LEFTPROCPAR RIGHTPROCPAR VIEW ONRETRIEVE ONUPDATE ONCREATE ONDELETE INDEX VIRTUAL COLON INTERFACE

%start statement

%right FIX_OP
%right SEMICOLON
%nonassoc RETURN
%nonassoc DELETE CREATE INSERTINTO
%left COMMA 
%right EXISTS FOR_ALL
%left GROUP_AS AS AS_INSTANCE_OF
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
%type <name_list> name_defs_semicolon
%type <privilige> privilige
%type <qtree> query
%type <proctree> procquery
%type <classtree> classquery
%type <classtree> classproc
%type <classtree> classprocs
%type <classtree> classbody
%type <proctree> formparams
%type <viewtree> viewquery
%type <viewtree> viewrecdef
%type <viewtree> viewdef
%type <proctree> viewproc
%type <fxtree> queryfixlist
%type <vectree> querycommalist
%type <tree> index_stmt
%type <tree> interface_stmt
%type <iStruct> interface_struct
%type <attributes> attributes
%type <attribute> attribute
%type <methods> methods
%type <method> method
%type <method_params> method_params
%type <method_param> method_param
%%

statement   : query semicolon_opt { d=$1; }
	    | BEGINTR semicolon_opt { d=new TransactNode(TransactNode::begin); }
	    | ABORT   semicolon_opt { d=new TransactNode(TransactNode::abort); }
	    | END     semicolon_opt { d=new TransactNode(TransactNode::end); }
		| RELOADSCHEME semicolon_opt { d=new DMLNode(DMLNode::reload); }	
	    | validate_stmt  semicolon_opt { d = $1; }
	    | privilige_stmt semicolon_opt { d = $1; }
	    | user_stmt	     semicolon_opt { d = $1; }
	    | index_stmt	 semicolon_opt { d = $1; }
	    | interface_stmt	semicolon_opt { d = $1; }
            ;

query	    : NAME { char *s = $1; $$ = new NameNode(s); delete s; }
	    | PARAMNAME { char *s = $1; $$ = new ParamNode(s); delete s; }
            | INTEGER { $$ = new IntNode($1); }
            | STRING { char *s = $1; $$ = new StringNode(s); delete s; }
	    | DOUBLE { $$ = new DoubleNode($1); }
	    | query AS_INSTANCE_OF query { $$ = new AsInstanceOfNode($1, $3); }
            | query AS NAME { $$ = new NameAsNode($1,$3,false); }
	    | query GROUP_AS NAME { $$ = new NameAsNode($1,$3,true); }
            | COUNT LEFTPAR  query RIGHTPAR { $$ = new UnOpNode($3,UnOpNode::count); }
	    | SUM LEFTPAR  query RIGHTPAR { $$ = new UnOpNode($3,UnOpNode::sum); }
	    | AVG LEFTPAR  query RIGHTPAR { $$ = new UnOpNode($3,UnOpNode::avg); }
	    | MIN LEFTPAR  query RIGHTPAR { $$ = new UnOpNode($3,UnOpNode::min); }
	    | MAX LEFTPAR  query RIGHTPAR { $$ = new UnOpNode($3,UnOpNode::max); }
	    | DEREF LEFTPAR  query RIGHTPAR  { $$ = new UnOpNode($3,UnOpNode::deref); }
	    | REF LEFTPAR query RIGHTPAR { $$ = new UnOpNode($3, UnOpNode::ref); }
	    | NAMEOF LEFTPAR query RIGHTPAR { $$ = new UnOpNode($3, UnOpNode::nameof); }
	    | DISTINCT LEFTPAR  query RIGHTPAR { $$ = new UnOpNode($3,UnOpNode::distinct); }
	    | MINUS query 				%prec UMINUS  { $$ = new UnOpNode($2,UnOpNode::unMinus); }
	    | NOT query { $$ = new UnOpNode($2,UnOpNode::boolNot); }
            | query UNION query { $$ = new AlgOpNode($1,$3,AlgOpNode::bagUnion); }
            | query INTERSECT query { $$ = new AlgOpNode($1,$3,AlgOpNode::bagIntersect); }
            | query EXCEPT query  { $$ = new AlgOpNode($1,$3,AlgOpNode::bagMinus); }
	    | LEFTPAR querycommalist RIGHTPAR { $$ = AlgOpNode::newNode($2, AlgOpNode::comma); } 
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
	    | FIXPOINT LEFTPAR queryfixlist RIGHTPAR { $$ = $3; }
	    | BREAK {$$ = new ReturnNode (); }
	    | RETURN query {$$ = new ReturnNode ($2); }
	    | CREATE procquery {$$ = new RegisterProcNode ($2);}
	    | CREATE viewquery {$$ = new RegisterViewNode ($2);}
        | CREATE classquery {$$ = new RegisterClassNode ($2);}
	    | NAME LEFTPAR RIGHTPAR {$$ = new CallProcNode ($1);}
	    | NAME LEFTPAR querycommalist RIGHTPAR {$$ = new CallProcNode ($1, $3);}
	    ;

queryfixlist   : NAME FIX_OP query { $$ = new FixPointNode ($1, $3); }
	    | queryfixlist NAME FIX_OP query {$$ = new FixPointNode ($2, $4, $1); }
	    ;

querycommalist  : query { $$ = new VectorNode ($1); }
	    | querycommalist COMMA query {$$ = new VectorNode ($3, $1); }
	    ;

procquery   : PROCEDURE NAME LEFTPAR formparams RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		char *n = $2; $4->addContent(n, $7); $$ = $4; delete n; }
	    | PROCEDURE NAME LEFTPAR RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		char *n = $2; $$ = new ProcedureNode(); $$->addContent(n, $6); delete n; }
            ;

formparams  : NAME {char *s = $1; $$ = new ProcedureNode (s); delete s;}
	    | formparams COMMA NAME {char *s = $3; $$ = new ProcedureNode (s, $1); delete s;}
	    ;

viewquery   : VIEW NAME LEFTPROCPAR VIRTUAL NAME LEFTPROCPAR query RIGHTPROCPAR viewrecdef RIGHTPROCPAR {
		char *n = $2; char *v = $5; $9->setName(n); $9->setVirtual(v,$7); $$ = $9; delete n; delete v; }
	    | VIEW NAME LEFTPROCPAR VIRTUAL NAME LEFTPROCPAR query RIGHTPROCPAR RIGHTPROCPAR { char *n = $2; char *v = $5; $$ = new ViewNode(n); $$->setVirtual(v,$7); delete n; delete v; }
	    | VIEW NAME LEFTPROCPAR VIRTUAL NAME LEFTPAR RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR viewrecdef RIGHTPROCPAR { char *n = $2; char *v = $5; $11->setName(n); $11->setVirtual(v,$9); $$ = $11; delete n; delete v; }
	    | VIEW NAME LEFTPROCPAR VIRTUAL NAME LEFTPAR RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR RIGHTPROCPAR { char *n = $2; char *v = $5; $$ = new ViewNode(n); $$->setVirtual(v,$9); delete n; delete v; }
            ;

viewrecdef  : viewdef		 { $$ = $1; }
            | viewrecdef viewdef { $1->addContent($2); }
            ;

viewdef     : viewproc	{ $$ = new ViewNode($1); }
            | viewquery	{ $$ = new ViewNode($1); }
            ;

viewproc    : procquery { $$ = $1; }
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
	    
index_stmt:	CREATE INDEX NAME ON NAME LEFTPAR NAME RIGHTPAR {$$ = new CreateIndexNode ($3, $5, $7);}
		|	INDEX {$$ = new ListIndexNode();}
		|	DELETE INDEX NAME {$$ = new DropIndexNode($3);}
		;

semicolon_opt:  /* empty */	{}
	    |	SEMICOLON	{}
	    ;
	    

interface_stmt: CREATE INTERFACE NAME LEFTPROCPAR interface_struct RIGHTPROCPAR {$$ = new CreateInterfaceNode($3, $5);}

interface_struct: LEFTPAR attributes RIGHTPAR semicolon_opt  methods { $$ = new InterfaceStruct ($2, $5);}
	| LEFTPAR attributes RIGHTPAR semicolon_opt { $$ = new InterfaceStruct ($2);}	
	;

attributes: attribute { $$ = new InterfaceAttributeListNode($1);}
	| attributes attribute { $$ = new InterfaceAttributeListNode($2, $1);}
	;

attribute: NAME COLON NAME SEMICOLON  {$$ = new InterfaceAttribute ($1, $3);}

methods: method { $$ = new InterfaceMethodListNode($1);}
	| methods method { $$ = new InterfaceMethodListNode($2, $1);}
	;

method: NAME LEFTPAR method_params RIGHTPAR SEMICOLON { $$ = new InterfaceMethod($1, $3);}

method_params: method_param { $$ = new InterfaceMethodParamListNode($1);} 
	| method_params COMMA method_param { $$ = new InterfaceMethodParamListNode($3, $1);}
	;

method_param: NAME COLON NAME { $$ = new InterfaceMethodParam($1, $3);};

classquery: CLASS NAME LEFTPROCPAR classbody RIGHTPROCPAR {char *n = $2; $4->setName(n); $$ = $4; delete n;}
    | CLASS NAME EXTENDS name_defs LEFTPROCPAR classbody RIGHTPROCPAR {
        char *n = $2; $6->setName(n); $6->setExtends($4); $$ = $6; delete n;};

classbody: /* empty */ { $$ = new ClassNode(); }
    /* procedure ... */
    | classprocs { $$ = $1 }
    /* instance name : { n1; n2 [;]} procedure ... */
    | INSTANCE NAME COLON LEFTPROCPAR name_defs_semicolon semicolon_opt RIGHTPROCPAR classprocs 
        {char *n = $2; $8->setInvariant(n); $8->setFields($5); $$ = $8; delete n;}
    /* instance : { n1; n2 [;]} procedure ... */
    | INSTANCE COLON LEFTPROCPAR name_defs_semicolon semicolon_opt RIGHTPROCPAR classprocs 
        {$7->setFields($4); $$ = $7;}
    /* instance name : {} procedure ... */
    | INSTANCE NAME COLON LEFTPROCPAR RIGHTPROCPAR classprocs 
        {char *n = $2; $6->setInvariant(n); $$ = $6; delete n;}
    /* instance name : { n1; n2 [;]} */
    | INSTANCE NAME COLON LEFTPROCPAR name_defs_semicolon semicolon_opt RIGHTPROCPAR 
        {char *n = $2; $$ = new ClassNode(); $$->setInvariant(n); $$->setFields($5); delete n;}
    /* instance : { n1; n2 [;]} */
    | INSTANCE COLON LEFTPROCPAR name_defs_semicolon semicolon_opt RIGHTPROCPAR
        {$$ = new ClassNode(); $$->setFields($4); }
    /* instance name : {} */
    | INSTANCE NAME COLON LEFTPROCPAR RIGHTPROCPAR
        {char *n = $2; $$ = new ClassNode(); $$->setInvariant(n); delete n;}
    ;

classproc: procquery { $$ = new ClassNode($1); }
    ;

classprocs: classproc { $$ = $1}
    | classprocs classproc { $1->addContent($2); }
    ;

name_defs_semicolon:  NAME { $$ = new NameListNode($1);    }
        |   name_defs SEMICOLON NAME { $$ = new NameListNode($3, $1);    }

//subclass: EXTENDS name_defs SEMICOLON {}

