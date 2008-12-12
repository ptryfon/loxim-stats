%pure-parser
%skeleton "lalr1.cc"
%defines
%define "parser_class_name" "QueryParserGen"
%name-prefix="QParser"

%{
#include <string>
#include <vector>
#include "TreeNode.h"
#include "IndexNode.h"
#include "Privilige.h"
#include "../Indexes/BTree.h"
  using namespace Indexes;
  using namespace QParser;
  class QueryLexer;
  int QParserlex(void *lval, QParser::QueryLexer *lexer);
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
  QParser::ClassCastNode* classcast;
  QParser::Privilige *privilige;
  QParser::PriviligeListNode *priv_list;
  QParser::NameListNode *name_list;
  QParser::InterfaceAttribute *attribute;
  QParser::InterfaceAttributes *attributes;
  QParser::InterfaceMethod* method;
  QParser::InterfaceMethods *methods;
  QParser::InterfaceMethodParams* method_params;
  QParser::InterfaceNode* interfacetree;
  QParser::InterfaceBind* iBind;
  QParser::Crud* crud;
  QParser::SchemaAPs* schema_aps;
  QParser::SchemaNode* schema;
  ComparatorNode* comparatorNode;
  IndexSelectNode* indexSelect;
  IndexBoundaryNode* IndexBoundary;

  QParser::SignatureNode* sigtree;
  QParser::StructureTypeNode* stucturetree;
  QParser::ObjectDeclareNode* obdecltree;
}

%lex-param{ QueryLexer *lexer }
%parse-param{ QueryLexer *lexer }
%parse-param{ TreeNode **result }


%nonassoc CREATE_OR_UPDATE
%token	<num> INTEGER
%token	<dbl> DOUBLE
%token	<str> STATIC SYSTEMVIEWNAME EXTNAME PARAMNAME NAME STRING SEMICOLON LEFTPAR RIGHTPAR SUM COUNT AVG MIN MAX DISTINCT DEREF REF NAMEOF RELOADSCHEME TCON TCOFF BEGINTR END ABORT CREATE IF FI DO OD ELSE WHILE LINK FOREACH THEN FIX_OP FIXPOINT RETURN BREAK VALIDATE READ MODIFY DELETE PASSWD WITH REVOKE REMOVE TO USER FROM ON GRANT OPTION PROCEDURE CLASS EXTENDS INSTANCE LEFTPROCPAR RIGHTPROCPAR VIEW ONRETRIEVE ONUPDATE ONCREATE ONDELETE ONINSERT ONNAVIGATE ONVITRUALIZE ONSTORE INDEX VIRTUAL COLON INTERFACE SCHEMA SHOWS ASSOCIATE CARDCONST TYPEDEF CARD00 CARD01 CARD 11 CARD0INF CARD1INF LEFTSQUAREPAR RIGHTSQUAREPAR STRING_SIG DOUBLE_SIG INTEGER_SIG BOOLEAN_SIG INT_SIG LEFT_EXCLUSIVE LEFT_INCLUSIVE RIGHT_EXCLUSIVE RIGHT_INCLUSIVE INDEXPAR THROW VIRTUALIZE_AS OBJECT_NAME_IS
		
%start statement
%expect 0

%right FIX_OP
%right SEMICOLON
%nonassoc RETURN
%nonassoc THROW
%nonassoc UPDATE DELETE CREATE INSERTINTO
%nonassoc TYPEDEF
%left COMMA 
%right EXISTS FOR_ALL
%left CAST GROUP_AS AS INCLUDES EXCLUDES INSTANCEOF
%right ASSIGN
%left UNION EXCEPT
%left INTERSECT
%right WHERE JOIN CLOSE_BY CLOSE_UNIQUE LEAVES_BY LEAVES_UNIQUE ORDER_BY VIRTUALIZE_AS
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
%type <name_list> classstatic
%type <privilige> privilige
%type <qtree> query
%type <proctree> procquery
%type <classtree> classquery
%type <classtree> classproc
%type <classtree> classprocs
%type <classtree> classbody
%type <classtree> classprocs_and_static
%type <proctree> formparams
%type <viewtree> viewquery
%type <viewtree> viewrecdef
%type <viewtree> viewdef
%type <proctree> viewproc
%type <fxtree> queryfixlist
%type <vectree> querycommalist
%type <tree> indexDDL_stmt
%type <qtree> indexDML_query
%type <comparatorNode> indexType
%type <indexSelect> index_constraints
%type <IndexBoundary> left_constraint
%type <IndexBoundary> right_constraint
%type <interfacetree> interface
%type <interfacetree> interface_struct
%type <attributes> attributes
%type <attribute> attribute
%type <methods> methods
%type <method> method
%type <crud> crud
%type <schema> schema
%type <schema_aps> schema_aps
//%type <schema_binds> schema_binds
%type <method_params> method_params
%type <iBind> interfaceBind
%type <tree> type_decl_stmt
%type <sigtree> signature
%type <obdecltree> object_decl
%type <stucturetree> object_decl_commalist
%%

statement   : query semicolon_opt { *result=$1; }
	    | BEGINTR semicolon_opt { *result=new TransactNode(TransactNode::begin); }
	    | ABORT   semicolon_opt { *result=new TransactNode(TransactNode::abort); }
	    | END     semicolon_opt { *result=new TransactNode(TransactNode::end); }
	    | RELOADSCHEME semicolon_opt { *result=new DMLNode(DMLNode::reload); }	
		| TCON	semicolon_opt { *result=new DMLNode(DMLNode::tcon); }
		| TCOFF semicolon_opt { *result=new DMLNode(DMLNode::tcoff);}
	    | validate_stmt  semicolon_opt { *result = $1; }
	    | privilige_stmt semicolon_opt { *result = $1; }
	    | user_stmt	     semicolon_opt { *result = $1; }
	    | indexDDL_stmt	 semicolon_opt { *result = $1; }
	    | interfaceBind	semicolon_opt { *result = $1;}
		| type_decl_stmt  semicolon_opt { *result = $1;}
	    ;

query	    : SYSTEMVIEWNAME { char *s = $1; $$ = new NameNode(s); delete s; }
		| NAME { char *s = $1; $$ = new NameNode(s); delete s; }
	    | EXTNAME { char *s = $1; $$ = new NameNode(s); delete s; }
	    | PARAMNAME { char *s = $1; $$ = new ParamNode(s); delete s; }
	    | INTEGER { $$ = new IntNode($1); }
    	| STRING { char *s = $1; $$ = new StringNode(s); delete s; }
	    | DOUBLE { $$ = new DoubleNode($1); }
	    | CREATE query AS EXTNAME { $$ = new CreateNode( new NameAsNode($2,$4,false), true); }
	    | query INCLUDES query { $$ = new IncludesNode($1, $3); }
	    | query EXCLUDES query { $$ = new ExcludesNode($1, $3); }
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
	    | IF query THEN query ELSE query FI { $$ = new CondNode($2, $4, $6,CondNode::ifElsee); }
	    | IF query THEN query FI { $$ = new CondNode($2, $4, CondNode::iff); }
	    | WHILE query DO query OD { $$ = new CondNode($2, $4, CondNode::whilee); }
	    | FOREACH query DO query OD { $$ = new NonAlgOpNode($2, $4, NonAlgOpNode::forEach); }
	    | LINK STRING STRING INTEGER { $$ = new LinkNode($2, $3, $4); }
	    | query SEMICOLON query { $$ = new AlgOpNode ($1, $3, AlgOpNode::semicolon); }
	    | FIXPOINT LEFTPAR queryfixlist RIGHTPAR { $$ = $3; }
	    | BREAK {$$ = new ReturnNode (); }
	    | RETURN query {$$ = new ReturnNode ($2); }
	    | CREATE query { $$ = new CreateNode($2); }
	    | CREATE procquery {$$ = new RegisterProcNode ($2);}
	    | CREATE viewquery {$$ = new RegisterViewNode ($2);}
	    | CREATE_OR_UPDATE classquery {$$ = new RegisterClassNode ($2, CT_CREATE_OR_UPDATE);}
	    | UPDATE classquery {$$ = new RegisterClassNode ($2, CT_UPDATE);}
	    | CREATE classquery {$$ = new RegisterClassNode ($2, CT_CREATE);}
	    | CREATE interface {$$ = new RegisterInterfaceNode ($2);}
		| CREATE schema {$$ = new RegisterSchemaNode ($2);}
	    | query INSERTINTO query { $$ = new AlgOpNode($1,$3,AlgOpNode::insert); }
	    | query INSERTINTO viewproc { $$ = new AlgOpNode($1,$3,AlgOpNode::insert_viewproc); }
	    | query INSERTINTO viewquery { $$ = new AlgOpNode($1,$3,AlgOpNode::insert_viewproc); }
	    | DELETE query  { $$ = new UnOpNode($2,UnOpNode::deleteOp); }
	    | query ASSIGN query { $$ = new AlgOpNode($1,$3,AlgOpNode::assign); }
	    | query ASSIGN viewproc { $$ = new AlgOpNode($1,$3,AlgOpNode::assign_viewproc); }
	    | query ASSIGN viewquery { $$ = new AlgOpNode($1,$3,AlgOpNode::assign_viewproc); }
	    | NAME LEFTPAR RIGHTPAR {$$ = new CallProcNode ($1);}
	    | NAME LEFTPAR querycommalist RIGHTPAR {$$ = new CallProcNode ($1, $3);}
	    | EXTNAME LEFTPAR RIGHTPAR {$$ = new CallProcNode ($1);}
	    | EXTNAME LEFTPAR querycommalist RIGHTPAR {$$ = new CallProcNode ($1, $3);}
	    | CAST LEFTPAR query AS LEFTPAR query RIGHTPAR RIGHTPAR {$$ = new ClassCastNode($3, $6);}
		| CAST LEFTPAR query TO signature RIGHTPAR {$$ = new SimpleCastNode($3, $5); }
	    | query INSTANCEOF query {$$ = new InstanceOfNode($1, $3);}
	    | indexDML_query
    	    | THROW query {$$ = new ThrowExceptionNode ($2); }
    	    | query VIRTUALIZE_AS NAME { $$ = new VirtualizeAsNode($1, $3); }
	    | query VIRTUALIZE_AS LEFTPAR query RIGHTPAR DOT NAME { $$ = new VirtualizeAsNode($1, $4, $7); }
    	    | ONUPDATE {$$ = new NameNode("on_update");}
    	    | ONCREATE {$$ = new NameNode("on_create");}
    	    | ONDELETE {$$ = new NameNode("on_delete");}
    	    | ONRETRIEVE {$$ = new NameNode("on_retrieve");}
    	    | ONINSERT {$$ = new NameNode("on_insert");}
    	    | ONNAVIGATE {$$ = new NameNode("on_navigate");}
    	    | ONVITRUALIZE {$$ = new NameNode("on_virtualize");}
    	    | ONSTORE {$$ = new NameNode("on_store");}
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
            | CREATE query { $$ = new ViewNode($2); }
            ;

viewproc    : procquery { $$ = $1; }
            | PROCEDURE ONUPDATE LEFTPAR NAME RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		$$ = new ProcedureNode(); char *p = $4; $$->addContent("on_update", $7); $$->addParam(p); delete p; }
            | PROCEDURE ONRETRIEVE LEFTPAR RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		$$ = new ProcedureNode(); $$->addContent("on_retrieve", $6); }
            | PROCEDURE ONCREATE LEFTPAR NAME RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		$$ = new ProcedureNode(); char *p = $4; $$->addContent("on_create", $7); $$->addParam(p); delete p; }
            | PROCEDURE ONDELETE LEFTPAR RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		$$ = new ProcedureNode(); $$->addContent("on_delete", $6); }
            | PROCEDURE ONINSERT LEFTPAR NAME RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		$$ = new ProcedureNode(); char *p = $4; $$->addContent("on_insert", $7); $$->addParam(p); delete p; }
            | PROCEDURE ONNAVIGATE LEFTPAR RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		$$ = new ProcedureNode(); $$->addContent("on_navigate", $6); }
            | PROCEDURE ONVITRUALIZE LEFTPAR NAME RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		$$ = new ProcedureNode(); char *p = $4; $$->addContent("on_virtualize", $7); $$->addParam(p); delete p; }
            | PROCEDURE ONSTORE LEFTPAR RIGHTPAR LEFTPROCPAR query RIGHTPROCPAR {
		$$ = new ProcedureNode(); $$->addContent("on_store", $6); }
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
	    
indexDDL_stmt:	CREATE indexType INDEX NAME ON NAME LEFTPAR NAME RIGHTPAR {$$ = new CreateIndexNode ($4, $6, $8, $2->comp);}
		|	INDEX {$$ = new ListIndexNode();}
		|	DELETE INDEX NAME {$$ = new DropIndexNode($3);}
		;

indexType:	INT_SIG		{$$ = new ComparatorNode(new IntComparator());}
		|	DOUBLE_SIG	{$$ = new ComparatorNode(new DoubleComparator());}
		|	STRING_SIG	{$$ = new ComparatorNode(new StringComparator());}
		;

indexDML_query: INDEX NAME index_constraints { $3->setIndexName(string($2)); $$ = $3;};



index_constraints:	
					EQUAL query {$$ = new IndexSelectNode($2);}
		|			left_constraint INDEXPAR {$$ = new IndexSelectNode($1, false);}
		|			INDEXPAR right_constraint {$$ = new IndexSelectNode(false, $2);}
		|			left_constraint TO right_constraint {$$ = new IndexSelectNode($1, $3);}
		;

left_constraint:	LEFT_INCLUSIVE query  {$$ = new IndexBoundaryNode(true, $2);}
		|			LEFT_EXCLUSIVE query  {$$ = new IndexBoundaryNode(false, $2);} 
		;

right_constraint:	query RIGHT_INCLUSIVE {$$ = new IndexBoundaryNode(true, $1);}
		|			query RIGHT_EXCLUSIVE {$$ = new IndexBoundaryNode(false, $1);}
		;

semicolon_opt:  /* empty */	{}
	    |	SEMICOLON	{}
	    ;


schema: SCHEMA NAME LEFTPROCPAR schema_aps RIGHTPROCPAR {$$ = new SchemaNode($2, $4->getAccessPoints());}

schema_aps: /*empty*/ {$$ = new SchemaAPs();}
	|   NAME crud schema_aps {$3->addAccessPoint($1, $2->getCrud()); $$=$3;}
	;
	
crud: /*empty*/ {$$ = new Crud();}
	|   privilige crud {$2->addAccessType($1->get_priv()); $$=$2;}
	;
	
	
	// schema_binds: /*empty*/ {$$ = new SchemaBinds();}
	//	|   NAME SHOWS NAME schema_binds {$4->addBind($1, $3); $$=$4;}
	//	;
	//
	    
interfaceBind: NAME SHOWS NAME {$$ = new InterfaceBind ($1, $3);};

interface: INTERFACE NAME LEFTPROCPAR INSTANCE NAME COLON interface_struct RIGHTPROCPAR {$7->setInterfaceName($2); $7->setObjectName($5); $$ = $7;}
	| INTERFACE NAME EXTENDS name_defs LEFTPROCPAR INSTANCE NAME COLON interface_struct RIGHTPROCPAR {$9->setInterfaceName($2); $9->setSupers($4); $9->setObjectName($7); $$=$9;}
	;

interface_struct: LEFTPROCPAR attributes RIGHTPROCPAR semicolon_opt  methods { InterfaceNode *n = new InterfaceNode(); n->setAttributes($2->getFields()); n->setMethods($5->getMethods()); $$=n;}
	| LEFTPROCPAR attributes RIGHTPROCPAR semicolon_opt { InterfaceNode *n = new InterfaceNode(); n->setAttributes($2->getFields()); $$=n;}
	| LEFTPROCPAR RIGHTPROCPAR semicolon_opt  methods { InterfaceNode *n = new InterfaceNode(); n->setMethods($4->getMethods()); $$=n;}
	;

attributes: attribute semicolon_opt { $$ = new InterfaceAttributes($1);}
	| attribute SEMICOLON attributes { $3->prependField($1); $$=$3;}
	;

attribute: NAME {$$ = new InterfaceAttribute ($1);};

methods: method semicolon_opt { $$ = new InterfaceMethods($1);}
	| method SEMICOLON methods { $3->prependMethod($1); $$ = $3;}
	;

method: NAME LEFTPAR RIGHTPAR { $$ = new InterfaceMethod($1);}
	| NAME LEFTPAR method_params RIGHTPAR { InterfaceMethod *m = new InterfaceMethod($1); m->addParams($3->getFields()); $$=m;}
	;
	
method_params: attribute { $$ = new InterfaceMethodParams($1);} 
	| attribute COMMA method_params { $3->prependField($1); $$ = $3;}
	;

classquery: CLASS NAME LEFTPROCPAR classbody RIGHTPROCPAR {char *n = $2; $4->setName(n); $$ = $4; delete n;}
    | CLASS NAME EXTENDS name_defs LEFTPROCPAR classbody RIGHTPROCPAR {
        char *n = $2; $6->setName(n); $6->setExtends($4); $$ = $6; delete n;};

classbody: /* empty */ { $$ = new ClassNode(); }
    /* procedure ... */
    | classprocs_and_static { $$ = $1; }
    /* instance name : { n1; n2 [;]} procedure ... */
    | INSTANCE NAME COLON LEFTPROCPAR name_defs_semicolon semicolon_opt RIGHTPROCPAR classprocs_and_static
        {char *n = $2; $8->setInvariant(n); $8->setFields($5); $$ = $8; delete n;}
    /* instance : { n1; n2 [;]} procedure ... */
    | INSTANCE COLON LEFTPROCPAR name_defs_semicolon semicolon_opt RIGHTPROCPAR classprocs_and_static 
        {$7->setFields($4); $$ = $7;}
    /* instance name : {} procedure ... */
    | INSTANCE NAME COLON LEFTPROCPAR RIGHTPROCPAR classprocs_and_static
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

classprocs_and_static: classstatic classprocs { $2->setStaticFields($1); $$ = $2; }
	| classstatic { $$ = new ClassNode(); $$->setStaticFields($1); }
	| classprocs {$$ = $1 ;}
	;

classstatic: STATIC LEFTPROCPAR name_defs_semicolon semicolon_opt RIGHTPROCPAR semicolon_opt
	{$$ = $3;};

classproc: procquery { $$ = new ClassNode($1); }
	| STATIC procquery { $$ = new ClassNode($2, true); }
    ;

classprocs: classproc { $$ = $1;}
    | classprocs classproc { $1->addContent($2); delete $2; }
    ;

type_decl_stmt: object_decl {$$ = $1;}
	| TYPEDEF NAME EQUAL signature {$$ = new TypeDefNode($2, $4, false);}
	| TYPEDEF DISTINCT NAME EQUAL signature {$$ = new TypeDefNode($3, $5, true);}
		/* other kinds of declarations may be added here...
	* like... PROGR_DECL? OR INTERFACE_DECL? OR CLASS_DECL - but for now, they've 
		* already been added elsewhere. */
	;
			
object_decl: NAME COLON signature {char *s = $1; $$ = new ObjectDeclareNode(s, $3); delete s;}
	| NAME LEFTSQUAREPAR CARDCONST RIGHTSQUAREPAR COLON signature
		{ char *s = $3; char *s2 = $1; $$ = new ObjectDeclareNode(s2, $6, s); delete s; delete s2;}
	;
				
object_decl_commalist : object_decl {$$ = new StructureTypeNode ($1); }
	| object_decl_commalist COMMA object_decl {$$ = new StructureTypeNode ($3, $1); }
	;
			
signature: STRING_SIG {$$ = new SignatureNode(SignatureNode::atomic, "string"); }
	| INTEGER_SIG {$$ = new SignatureNode(SignatureNode::atomic, "int");}
	| DOUBLE_SIG {$$ = new SignatureNode(SignatureNode::atomic, "double");}	
	| REF NAME {char *s = $2; $$ = new SignatureNode(SignatureNode::ref, s); delete s;}
	| NAME {char *s = $1; $$ = new SignatureNode(SignatureNode::defType, s); delete s;} 
	/** defined types OR defined distinct types. */
	| LEFTPAR object_decl_commalist RIGHTPAR {$$ = new SignatureNode($2);}
	;
	
name_defs_semicolon:  NAME { $$ = new NameListNode($1);    }
        |   name_defs_semicolon SEMICOLON NAME { $$ = new NameListNode($3, $1);    }

//subclass: EXTENDS name_defs SEMICOLON {}
%%

void QParser::QueryParserGen::error(const QParser::QueryParserGen::location_type& l, const std::string& m)
{

}

