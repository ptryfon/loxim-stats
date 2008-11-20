/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison LALR(1) parsers in C++

   Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C++ LALR(1) parser skeleton written by Akim Demaille.  */

#ifndef PARSER_HEADER_H
# define PARSER_HEADER_H

#include <string>
#include <iostream>
#include "stack.hh"

namespace QParser
{
  class position;
  class location;
}

/* First part of user declarations.  */
#line 7 "parser.y"

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


/* Line 35 of lalr1.cc.  */
#line 67 "y.tab.h"

#include "location.hh"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
do {							\
  if (N)						\
    {							\
      (Current).begin = (Rhs)[1].begin;			\
      (Current).end   = (Rhs)[N].end;			\
    }							\
  else							\
    {							\
      (Current).begin = (Current).end = (Rhs)[0].end;	\
    }							\
} while (false)
#endif

namespace QParser
{

  /// A Bison parser.
  class QueryParserGen
  {
  public:
    /// Symbol semantic values.
#ifndef YYSTYPE
    union semantic_type
#line 20 "parser.y"
{
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
  ComparatorNode* comparatorNode;
  IndexSelectNode* indexSelect;
  IndexBoundaryNode* IndexBoundary;
  QParser::InterfaceNode* interfacetree;
  QParser::InterfaceBind* iBind;
  QParser::SignatureNode* sigtree;
  QParser::StructureTypeNode* stucturetree;
  QParser::ObjectDeclareNode* obdecltree;
}
/* Line 35 of lalr1.cc.  */
#line 150 "y.tab.h"
	;
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;
    /// Tokens.
    struct token
    {
      /* Tokens.  */
   enum yytokentype {
     CREATE_OR_UPDATE = 258,
     INTEGER = 259,
     DOUBLE = 260,
     STATIC = 261,
     SYSTEMVIEWNAME = 262,
     EXTNAME = 263,
     PARAMNAME = 264,
     NAME = 265,
     STRING = 266,
     SEMICOLON = 267,
     LEFTPAR = 268,
     RIGHTPAR = 269,
     SUM = 270,
     COUNT = 271,
     AVG = 272,
     MIN = 273,
     MAX = 274,
     DISTINCT = 275,
     DEREF = 276,
     REF = 277,
     NAMEOF = 278,
     RELOADSCHEME = 279,
     TCON = 280,
     TCOFF = 281,
     BEGINTR = 282,
     END = 283,
     ABORT = 284,
     CREATE = 285,
     IF = 286,
     FI = 287,
     DO = 288,
     OD = 289,
     ELSE = 290,
     WHILE = 291,
     LINK = 292,
     FOREACH = 293,
     THEN = 294,
     FIX_OP = 295,
     FIXPOINT = 296,
     RETURN = 297,
     BREAK = 298,
     VALIDATE = 299,
     READ = 300,
     MODIFY = 301,
     DELETE = 302,
     PASSWD = 303,
     WITH = 304,
     REVOKE = 305,
     REMOVE = 306,
     TO = 307,
     USER = 308,
     FROM = 309,
     ON = 310,
     GRANT = 311,
     OPTION = 312,
     PROCEDURE = 313,
     CLASS = 314,
     EXTENDS = 315,
     INSTANCE = 316,
     LEFTPROCPAR = 317,
     RIGHTPROCPAR = 318,
     VIEW = 319,
     ONRETRIEVE = 320,
     ONUPDATE = 321,
     ONCREATE = 322,
     ONDELETE = 323,
     ONINSERT = 324,
     ONNAVIGATE = 325,
     ONVITRUALIZE = 326,
     ONSTORE = 327,
     INDEX = 328,
     VIRTUAL = 329,
     COLON = 330,
     INTERFACE = 331,
     SHOWS = 332,
     ASSOCIATE = 333,
     CARDCONST = 334,
     TYPEDEF = 335,
     CARD00 = 336,
     CARD01 = 337,
     CARD = 11,
     CARD0INF = 338,
     CARD1INF = 339,
     LEFTSQUAREPAR = 340,
     RIGHTSQUAREPAR = 341,
     STRING_SIG = 342,
     DOUBLE_SIG = 343,
     INTEGER_SIG = 344,
     BOOLEAN_SIG = 345,
     INT_SIG = 346,
     LEFT_EXCLUSIVE = 347,
     LEFT_INCLUSIVE = 348,
     RIGHT_EXCLUSIVE = 349,
     RIGHT_INCLUSIVE = 350,
     INDEXPAR = 351,
     THROW = 352,
     VIRTUALIZE_AS = 353,
     OBJECT_NAME_IS = 354,
     INSERTINTO = 355,
     UPDATE = 356,
     COMMA = 357,
     FOR_ALL = 358,
     EXISTS = 359,
     INSTANCEOF = 360,
     EXCLUDES = 361,
     INCLUDES = 362,
     AS = 363,
     GROUP_AS = 364,
     CAST = 365,
     ASSIGN = 366,
     EXCEPT = 367,
     UNION = 368,
     INTERSECT = 369,
     ORDER_BY = 370,
     LEAVES_UNIQUE = 371,
     LEAVES_BY = 372,
     CLOSE_UNIQUE = 373,
     CLOSE_BY = 374,
     JOIN = 375,
     WHERE = 376,
     OR = 377,
     AND = 378,
     NOT = 379,
     LESS_EQ = 380,
     GREATER_EQ = 381,
     LESS_THAN = 382,
     GREATER_THAN = 383,
     NOT_EQUAL = 384,
     REFEQUAL = 385,
     EQUAL = 386,
     MINUS = 387,
     PLUS = 388,
     DIVIDE_BY = 389,
     TIMES = 390,
     UMINUS = 391,
     DOT = 392
   };

    };
    /// Token type.
    typedef token::yytokentype token_type;

    /// Build a parser object.
    QueryParserGen (QueryLexer *lexer_yyarg, TreeNode **result_yyarg);
    virtual ~QueryParserGen ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

    /// The current debugging stream.
    std::ostream& debug_stream () const;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);

  private:
    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Generate an error message.
    /// \param state   the state where the error occurred.
    /// \param tok     the look-ahead token.
    virtual std::string yysyntax_error_ (int yystate);

#if YYDEBUG
    /// \brief Report a symbol value on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_value_print_ (int yytype,
					 const semantic_type* yyvaluep,
					 const location_type* yylocationp);
    /// \brief Report a symbol on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_print_ (int yytype,
				   const semantic_type* yyvaluep,
				   const location_type* yylocationp);
#endif /* ! YYDEBUG */


    /// State numbers.
    typedef int state_type;
    /// State stack type.
    typedef stack<state_type>    state_stack_type;
    /// Semantic value stack type.
    typedef stack<semantic_type> semantic_stack_type;
    /// location stack type.
    typedef stack<location_type> location_stack_type;

    /// The state stack.
    state_stack_type yystate_stack_;
    /// The semantic value stack.
    semantic_stack_type yysemantic_stack_;
    /// The location stack.
    location_stack_type yylocation_stack_;

    /// Internal symbol numbers.
    typedef unsigned char token_number_type;
    /* Tables.  */
    /// For a state, the index in \a yytable_ of its portion.
    static const short int yypact_[];
    static const short int yypact_ninf_;

    /// For a state, default rule to reduce.
    /// Unless\a  yytable_ specifies something else to do.
    /// Zero means the default is an error.
    static const unsigned char yydefact_[];

    static const short int yypgoto_[];
    static const short int yydefgoto_[];

    /// What to do in a state.
    /// \a yytable_[yypact_[s]]: what to do in state \a s.
    /// - if positive, shift that token.
    /// - if negative, reduce the rule which number is the opposite.
    /// - if zero, do what YYDEFACT says.
    static const short int yytable_[];
    static const signed char yytable_ninf_;

    static const short int yycheck_[];

    /// For a state, its accessing symbol.
    static const unsigned char yystos_[];

    /// For a rule, its LHS.
    static const unsigned char yyr1_[];
    /// For a rule, its RHS length.
    static const unsigned char yyr2_[];

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#endif

#if YYERROR_VERBOSE
    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    virtual std::string yytnamerr_ (const char *n);
#endif

#if YYDEBUG
    /// A type to store symbol numbers and -1.
    typedef short int rhs_number_type;
    /// A `-1'-separated list of the rules' RHS.
    static const rhs_number_type yyrhs_[];
    /// For each rule, the index of the first RHS symbol in \a yyrhs_.
    static const unsigned short int yyprhs_[];
    /// For each rule, its source line number.
    static const unsigned short int yyrline_[];
    /// For each scanner token number, its symbol number.
    static const unsigned short int yytoken_number_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();
#endif

    /// Convert a scanner token number \a t to a symbol number.
    token_number_type yytranslate_ (int t);

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg        Why this token is reclaimed.
    /// \param yytype       The symbol type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    inline void yydestruct_ (const char* yymsg,
			     int yytype,
			     semantic_type* yyvaluep,
			     location_type* yylocationp);

    /// Pop \a n symbols the three stacks.
    inline void yypop_ (unsigned int n = 1);

    /* Constants.  */
    static const int yyeof_;
    /* LAST_ -- Last index in TABLE_.  */
    static const int yylast_;
    static const int yynnts_;
    static const int yyempty_;
    static const int yyfinal_;
    static const int yyterror_;
    static const int yyerrcode_;
    static const int yyntokens_;
    static const unsigned int yyuser_token_number_max_;
    static const token_number_type yyundef_token_;

    /* Debugging.  */
    int yydebug_;
    std::ostream* yycdebug_;


    /* User arguments.  */
    QueryLexer *lexer;
    TreeNode **result;
  };
}


#endif /* ! defined PARSER_HEADER_H */
