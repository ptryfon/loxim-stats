%token	INTEGER
%token	DOUBLE 
%token	NAME STRING SEMICOLON LEFTPAR RIGHTPAR SUM COUNT AVG MIN
%token	MAX DISTINCT INSERT INTO  DEREF BEGIN END ABORT CREATE

%start statement

%nonassoc DELETE CREATE INTO
%left COMMA
%right EXISTS FOR_ALL
%left GROUP_AS AS
%right ASSIGN
%left UNION EXCEPT
%left INTERSECT
%left WHERE JOIN CLOSE_BY CLOSE_UNIQUE LEAVES_BY LEAVES_UNIQUE ORDER_BY
%left OR
%left AND
%nonassoc NOT
%nonassoc EQUAL NOT_EQUAL GREATER_THAN LESS_THAN GREATER_EQ LESS_EQ
%left PLUS MINUS
%left TIMES DIVIDE_BY
%nonassoc UMINUS
%left DOT

%%

statement : query SEMICOLON
	  | BEGIN SEMICOLON
	  | ABORT SEMICOLON
	  | END   SEMICOLON;

query	  : NAME 
          | INTEGER
          | STRING
	  | DOUBLE
          | LEFTPAR query RIGHTPAR
          | query AS       NAME 
	  | query GROUP_AS NAME 
          | COUNT    LEFTPAR query RIGHTPAR 
	  | SUM      LEFTPAR query RIGHTPAR
	  | AVG      LEFTPAR query RIGHTPAR
	  | MIN      LEFTPAR query RIGHTPAR 
	  | MAX      LEFTPAR query RIGHTPAR
	  | DEREF    LEFTPAR query RIGHTPAR
	  | DISTINCT LEFTPAR query RIGHTPAR 
	  | MINUS query 			%prec UMINUS
	  | NOT   query
          | query	UNION		query
          | query	INTERSECT	query
          | query	EXCEPT		query 
	  | query	COMMA		query 
	  | query	PLUS		query 
          | query	MINUS		query 
	  | query	TIMES		query 
	  | query	DIVIDE_BY	query 
	  | query	EQUAL		query 
	  | query	NOT_EQUAL	query 
	  | query	GREATER_THAN	query 
	  | query	LESS_THAN	query 
	  | query	GREATER_EQ	query 
	  | query	LESS_EQ		query 
	  | query	AND		query
	  | query	OR		query
	  | query 	DOT		query
	  | query 	JOIN		query
	  | query 	WHERE		query
	  | query 	CLOSE_BY	query 
	  | query 	CLOSE_UNIQUE	query 
	  | query 	LEAVES_BY	query 
	  | query 	LEAVES_UNIQUE	query 
	  | query 	ORDER_BY	query
	  | query 	EXISTS		query
	  | query 	FOR_ALL		query
	  | query	ASSIGN		query
	  | CREATE NAME LEFTPAR query RIGHTPAR
	  | CREATE NAME 
	  | INSERT query INTO query
	  | DELETE query 
 	  ;
