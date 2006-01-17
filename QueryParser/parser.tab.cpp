/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER = 258,
     DOUBLE = 259,
     NAME = 260,
     STRING = 261,
     SEMICOLON = 262,
     LEFTPAR = 263,
     RIGHTPAR = 264,
     SUM = 265,
     COUNT = 266,
     AVG = 267,
     MIN = 268,
     MAX = 269,
     DISTINCT = 270,
     INSERT = 271,
     INTO = 272,
     DEREF = 273,
     BEGINTR = 274,
     END = 275,
     ABORT = 276,
     CREATE = 277,
     DELETE = 278,
     COMMA = 279,
     FOR_ALL = 280,
     EXISTS = 281,
     AS = 282,
     GROUP_AS = 283,
     ASSIGN = 284,
     EXCEPT = 285,
     UNION = 286,
     INTERSECT = 287,
     ORDER_BY = 288,
     LEAVES_UNIQUE = 289,
     LEAVES_BY = 290,
     CLOSE_UNIQUE = 291,
     CLOSE_BY = 292,
     JOIN = 293,
     WHERE = 294,
     OR = 295,
     AND = 296,
     NOT = 297,
     LESS_EQ = 298,
     GREATER_EQ = 299,
     LESS_THAN = 300,
     GREATER_THAN = 301,
     NOT_EQUAL = 302,
     EQUAL = 303,
     MINUS = 304,
     PLUS = 305,
     DIVIDE_BY = 306,
     TIMES = 307,
     UMINUS = 308,
     DOT = 309
   };
#endif
#define INTEGER 258
#define DOUBLE 259
#define NAME 260
#define STRING 261
#define SEMICOLON 262
#define LEFTPAR 263
#define RIGHTPAR 264
#define SUM 265
#define COUNT 266
#define AVG 267
#define MIN 268
#define MAX 269
#define DISTINCT 270
#define INSERT 271
#define INTO 272
#define DEREF 273
#define BEGINTR 274
#define END 275
#define ABORT 276
#define CREATE 277
#define DELETE 278
#define COMMA 279
#define FOR_ALL 280
#define EXISTS 281
#define AS 282
#define GROUP_AS 283
#define ASSIGN 284
#define EXCEPT 285
#define UNION 286
#define INTERSECT 287
#define ORDER_BY 288
#define LEAVES_UNIQUE 289
#define LEAVES_BY 290
#define CLOSE_UNIQUE 291
#define CLOSE_BY 292
#define JOIN 293
#define WHERE 294
#define OR 295
#define AND 296
#define NOT 297
#define LESS_EQ 298
#define GREATER_EQ 299
#define LESS_THAN 300
#define GREATER_THAN 301
#define NOT_EQUAL 302
#define EQUAL 303
#define MINUS 304
#define PLUS 305
#define DIVIDE_BY 306
#define TIMES 307
#define UMINUS 308
#define DOT 309




/* Copy the first part of user declarations.  */
#line 1 "parser.y"

#include <string>
#include <vector>
#include "TreeNode.h"
  using namespace QParser;
  int yylex();
  int yyerror(char* s);
  TreeNode *d;


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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 11 "parser.y"
typedef union YYSTYPE {
  char* str;
  int num;
  double dbl;
  QParser::TreeNode* tree;
  QParser::QueryNode* qtree;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 201 "parser.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 213 "parser.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  39
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   649

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  3
/* YYNRULES -- Number of rules. */
#define YYNRULES  51
/* YYNRULES -- Number of states. */
#define YYNSTATES  117

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   309

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     6,     9,    12,    15,    17,    19,    21,
      23,    27,    31,    35,    40,    45,    50,    55,    60,    65,
      70,    73,    76,    80,    84,    88,    92,    96,   100,   104,
     108,   112,   116,   120,   124,   128,   132,   136,   140,   144,
     148,   152,   156,   160,   164,   168,   172,   176,   180,   186,
     189,   194
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      56,     0,    -1,    57,     7,    -1,    19,     7,    -1,    21,
       7,    -1,    20,     7,    -1,     5,    -1,     3,    -1,     6,
      -1,     4,    -1,     8,    57,     9,    -1,    57,    27,     5,
      -1,    57,    28,     5,    -1,    11,     8,    57,     9,    -1,
      10,     8,    57,     9,    -1,    12,     8,    57,     9,    -1,
      13,     8,    57,     9,    -1,    14,     8,    57,     9,    -1,
      18,     8,    57,     9,    -1,    15,     8,    57,     9,    -1,
      49,    57,    -1,    42,    57,    -1,    57,    31,    57,    -1,
      57,    32,    57,    -1,    57,    30,    57,    -1,    57,    24,
      57,    -1,    57,    50,    57,    -1,    57,    49,    57,    -1,
      57,    52,    57,    -1,    57,    51,    57,    -1,    57,    48,
      57,    -1,    57,    47,    57,    -1,    57,    46,    57,    -1,
      57,    45,    57,    -1,    57,    44,    57,    -1,    57,    43,
      57,    -1,    57,    41,    57,    -1,    57,    40,    57,    -1,
      57,    54,    57,    -1,    57,    38,    57,    -1,    57,    39,
      57,    -1,    57,    37,    57,    -1,    57,    36,    57,    -1,
      57,    35,    57,    -1,    57,    34,    57,    -1,    57,    33,
      57,    -1,    57,    26,    57,    -1,    57,    25,    57,    -1,
      22,     5,     8,    57,     9,    -1,    22,     5,    -1,    16,
      57,    17,    57,    -1,    23,    57,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    47,    47,    48,    49,    50,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INTEGER", "DOUBLE", "NAME", "STRING", 
  "SEMICOLON", "LEFTPAR", "RIGHTPAR", "SUM", "COUNT", "AVG", "MIN", "MAX", 
  "DISTINCT", "INSERT", "INTO", "DEREF", "BEGINTR", "END", "ABORT", 
  "CREATE", "DELETE", "COMMA", "FOR_ALL", "EXISTS", "AS", "GROUP_AS", 
  "ASSIGN", "EXCEPT", "UNION", "INTERSECT", "ORDER_BY", "LEAVES_UNIQUE", 
  "LEAVES_BY", "CLOSE_UNIQUE", "CLOSE_BY", "JOIN", "WHERE", "OR", "AND", 
  "NOT", "LESS_EQ", "GREATER_EQ", "LESS_THAN", "GREATER_THAN", 
  "NOT_EQUAL", "EQUAL", "MINUS", "PLUS", "DIVIDE_BY", "TIMES", "UMINUS", 
  "DOT", "$accept", "statement", "query", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    55,    56,    56,    56,    56,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     2,     2,     2,     1,     1,     1,     1,
       3,     3,     3,     4,     4,     4,     4,     4,     4,     4,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     5,     2,
       4,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     7,     9,     6,     8,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     3,     5,     4,    49,    51,    21,    20,     1,
       2,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    10,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    25,
      47,    46,    11,    12,    24,    22,    23,    45,    44,    43,
      42,    41,    39,    40,    37,    36,    35,    34,    33,    32,
      31,    30,    27,    26,    29,    28,    38,    14,    13,    15,
      16,    17,    19,    50,    18,     0,    48
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,    21,    22
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -51
static const short yypact[] =
{
     109,   -51,   -51,   -51,   -51,   130,    -3,    -2,     0,     1,
       2,     3,   130,     4,    -4,     9,    10,    13,   130,   130,
     130,    25,    57,   150,   130,   130,   130,   130,   130,   130,
     450,   130,   -51,   -51,   -51,    19,   481,   583,   -26,   -51,
     -51,   130,   130,   130,    24,    26,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   -51,
     183,   216,   249,   282,   315,   348,   130,   381,   130,   511,
     511,   511,   -51,   -51,   534,   534,   556,   571,   571,   571,
     571,   571,   571,   571,   119,   583,   595,   595,   595,   595,
     595,   595,   -50,   -50,   -26,   -26,   -51,   -51,   -51,   -51,
     -51,   -51,   -51,   481,   -51,   414,   -51
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -51,   -51,    -5
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yysigned_char yytable[] =
{
      23,    66,    67,    32,    68,    24,    25,    30,    26,    27,
      28,    29,    31,    36,    37,    38,    33,    34,    35,    70,
      71,    72,    73,    74,    75,    39,    77,    78,    68,    82,
       0,    83,     0,     0,     0,     0,    79,    80,    81,     0,
       0,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,    40,     0,     0,     0,     0,     0,
       0,   113,     0,   115,     0,     0,     0,     0,     0,     0,
       0,    41,    42,    43,    44,    45,     0,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,     0,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,    68,     1,     2,     3,     4,     0,     5,     0,     6,
       7,     8,     9,    10,    11,    12,     0,    13,    14,    15,
      16,    17,    18,     1,     2,     3,     4,     0,     5,     0,
       6,     7,     8,     9,    10,    11,    12,     0,    13,     0,
       0,    19,    17,    18,     0,     0,     0,     0,    20,    69,
      57,     0,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    19,    68,    41,    42,    43,    44,    45,    20,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,   107,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     0,    68,     0,     0,    41,    42,    43,
      44,    45,     0,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,   108,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,     0,     0,
      41,    42,    43,    44,    45,     0,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,   109,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,     0,     0,    41,    42,    43,    44,    45,     0,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,   110,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,    68,     0,     0,    41,    42,    43,    44,
      45,     0,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,   111,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     0,    68,     0,     0,    41,
      42,    43,    44,    45,     0,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,   112,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,    68,
       0,     0,    41,    42,    43,    44,    45,     0,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
     114,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    68,     0,     0,    41,    42,    43,    44,    45,
       0,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,   116,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,     0,    68,     0,     0,    41,    42,
      43,    44,    45,     0,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,     0,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    76,    68,     0,
       0,     0,     0,     0,    41,    42,    43,    44,    45,     0,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,     0,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     0,    68,    41,    42,    43,    44,    45,
       0,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,     0,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,     0,    68,    42,    43,    44,    45,
       0,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,     0,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,     0,    68,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,     0,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,     0,    68,    49,
      50,    51,    52,    53,    54,    55,    56,    57,     0,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,    56,    57,     0,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,     0,    68,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    64,    65,    66,    67,     0,    68
};

static const yysigned_char yycheck[] =
{
       5,    51,    52,     7,    54,     8,     8,    12,     8,     8,
       8,     8,     8,    18,    19,    20,     7,     7,     5,    24,
      25,    26,    27,    28,    29,     0,    31,     8,    54,     5,
      -1,     5,    -1,    -1,    -1,    -1,    41,    42,    43,    -1,
      -1,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    76,    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    24,    25,    26,    27,    28,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      -1,    54,     3,     4,     5,     6,    -1,     8,    -1,    10,
      11,    12,    13,    14,    15,    16,    -1,    18,    19,    20,
      21,    22,    23,     3,     4,     5,     6,    -1,     8,    -1,
      10,    11,    12,    13,    14,    15,    16,    -1,    18,    -1,
      -1,    42,    22,    23,    -1,    -1,    -1,    -1,    49,     9,
      41,    -1,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    42,    54,    24,    25,    26,    27,    28,    49,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,     9,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    -1,    54,    -1,    -1,    24,    25,    26,
      27,    28,    -1,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,     9,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    -1,    54,    -1,    -1,
      24,    25,    26,    27,    28,    -1,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,     9,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    -1,
      54,    -1,    -1,    24,    25,    26,    27,    28,    -1,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,     9,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    -1,    54,    -1,    -1,    24,    25,    26,    27,
      28,    -1,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,     9,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    -1,    54,    -1,    -1,    24,
      25,    26,    27,    28,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,     9,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    -1,    54,
      -1,    -1,    24,    25,    26,    27,    28,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
       9,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    -1,    54,    -1,    -1,    24,    25,    26,    27,    28,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,     9,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    -1,    54,    -1,    -1,    24,    25,
      26,    27,    28,    -1,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    -1,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    17,    54,    -1,
      -1,    -1,    -1,    -1,    24,    25,    26,    27,    28,    -1,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    -1,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    -1,    54,    24,    25,    26,    27,    28,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    -1,    54,    25,    26,    27,    28,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    -1,    54,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    -1,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    -1,    54,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    -1,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    -1,
      54,    40,    41,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    -1,    54,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    -1,    54,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    -1,    54
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,     4,     5,     6,     8,    10,    11,    12,    13,
      14,    15,    16,    18,    19,    20,    21,    22,    23,    42,
      49,    56,    57,    57,     8,     8,     8,     8,     8,     8,
      57,     8,     7,     7,     7,     5,    57,    57,    57,     0,
       7,    24,    25,    26,    27,    28,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    54,     9,
      57,    57,    57,    57,    57,    57,    17,    57,     8,    57,
      57,    57,     5,     5,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,     9,     9,     9,
       9,     9,     9,    57,     9,    57,     9
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 47 "parser.y"
    { d=yyvsp[-1].qtree; ;}
    break;

  case 3:
#line 48 "parser.y"
    { d=new TransactNode(TransactNode::begin); ;}
    break;

  case 4:
#line 49 "parser.y"
    { d=new TransactNode(TransactNode::abort); ;}
    break;

  case 5:
#line 50 "parser.y"
    { d=new TransactNode(TransactNode::end); ;}
    break;

  case 6:
#line 53 "parser.y"
    { char *s = yyvsp[0].str; yyval.qtree = new NameNode(s); delete s; ;}
    break;

  case 7:
#line 54 "parser.y"
    { yyval.qtree = new IntNode(yyvsp[0].num); ;}
    break;

  case 8:
#line 55 "parser.y"
    { char *s = yyvsp[0].str; yyval.qtree = new StringNode(s); delete s; ;}
    break;

  case 9:
#line 56 "parser.y"
    { yyval.qtree = new DoubleNode(yyvsp[0].dbl); ;}
    break;

  case 10:
#line 57 "parser.y"
    { yyval.qtree = yyvsp[-1].qtree; ;}
    break;

  case 11:
#line 58 "parser.y"
    { yyval.qtree = new NameAsNode(yyvsp[-2].qtree,yyvsp[0].str,false); ;}
    break;

  case 12:
#line 59 "parser.y"
    { yyval.qtree = new NameAsNode(yyvsp[-2].qtree,yyvsp[0].str,true); ;}
    break;

  case 13:
#line 60 "parser.y"
    { yyval.qtree = new UnOpNode(yyvsp[-1].qtree,UnOpNode::count); ;}
    break;

  case 14:
#line 61 "parser.y"
    { yyval.qtree = new UnOpNode(yyvsp[-1].qtree,UnOpNode::sum); ;}
    break;

  case 15:
#line 62 "parser.y"
    { yyval.qtree = new UnOpNode(yyvsp[-1].qtree,UnOpNode::avg); ;}
    break;

  case 16:
#line 63 "parser.y"
    { yyval.qtree = new UnOpNode(yyvsp[-1].qtree,UnOpNode::min); ;}
    break;

  case 17:
#line 64 "parser.y"
    { yyval.qtree = new UnOpNode(yyvsp[-1].qtree,UnOpNode::max); ;}
    break;

  case 18:
#line 65 "parser.y"
    { yyval.qtree = new UnOpNode(yyvsp[-1].qtree,UnOpNode::deref); ;}
    break;

  case 19:
#line 66 "parser.y"
    { yyval.qtree = new UnOpNode(yyvsp[-1].qtree,UnOpNode::distinct); ;}
    break;

  case 20:
#line 67 "parser.y"
    { yyval.qtree = new UnOpNode(yyvsp[0].qtree,UnOpNode::unMinus); ;}
    break;

  case 21:
#line 68 "parser.y"
    { yyval.qtree = new UnOpNode(yyvsp[0].qtree,UnOpNode::boolNot); ;}
    break;

  case 22:
#line 69 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::bagUnion); ;}
    break;

  case 23:
#line 70 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::bagIntersect); ;}
    break;

  case 24:
#line 71 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::bagMinus); ;}
    break;

  case 25:
#line 72 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::comma); ;}
    break;

  case 26:
#line 73 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::plus); ;}
    break;

  case 27:
#line 74 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::minus); ;}
    break;

  case 28:
#line 75 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::times); ;}
    break;

  case 29:
#line 76 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::divide); ;}
    break;

  case 30:
#line 77 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::eq); ;}
    break;

  case 31:
#line 78 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::neq); ;}
    break;

  case 32:
#line 79 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::gt); ;}
    break;

  case 33:
#line 80 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::lt); ;}
    break;

  case 34:
#line 81 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::ge); ;}
    break;

  case 35:
#line 82 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::le); ;}
    break;

  case 36:
#line 83 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::boolAnd); ;}
    break;

  case 37:
#line 84 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::boolOr); ;}
    break;

  case 38:
#line 85 "parser.y"
    { yyval.qtree = new NonAlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,NonAlgOpNode::dot); ;}
    break;

  case 39:
#line 86 "parser.y"
    { yyval.qtree = new NonAlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,NonAlgOpNode::join); ;}
    break;

  case 40:
#line 87 "parser.y"
    { yyval.qtree = new NonAlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,NonAlgOpNode::where); ;}
    break;

  case 41:
#line 88 "parser.y"
    { yyval.qtree = new NonAlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,NonAlgOpNode::closeBy); ;}
    break;

  case 42:
#line 89 "parser.y"
    { yyval.qtree = new NonAlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,NonAlgOpNode::closeUniqueBy); ;}
    break;

  case 43:
#line 90 "parser.y"
    { yyval.qtree = new NonAlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,NonAlgOpNode::leavesBy); ;}
    break;

  case 44:
#line 91 "parser.y"
    { yyval.qtree = new NonAlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,NonAlgOpNode::leavesUniqueBy); ;}
    break;

  case 45:
#line 92 "parser.y"
    { yyval.qtree = new NonAlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,NonAlgOpNode::orderBy); ;}
    break;

  case 46:
#line 93 "parser.y"
    { yyval.qtree = new NonAlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,NonAlgOpNode::exists); ;}
    break;

  case 47:
#line 94 "parser.y"
    { yyval.qtree = new NonAlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,NonAlgOpNode::forAll); ;}
    break;

  case 48:
#line 95 "parser.y"
    { yyval.qtree = new CreateNode(yyvsp[-3].str,yyvsp[-1].qtree); ;}
    break;

  case 49:
#line 96 "parser.y"
    { yyval.qtree = new CreateNode(yyvsp[0].str); ;}
    break;

  case 50:
#line 97 "parser.y"
    { yyval.qtree = new AlgOpNode(yyvsp[-2].qtree,yyvsp[0].qtree,AlgOpNode::insert); ;}
    break;

  case 51:
#line 98 "parser.y"
    { yyval.qtree = new UnOpNode(yyvsp[0].qtree,UnOpNode::deleteOp); ;}
    break;


    }

/* Line 991 of yacc.c.  */
#line 1540 "parser.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab2;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:

  /* Suppress GCC warning that yyerrlab1 is unused when no action
     invokes YYERROR.  */
#if defined (__GNUC_MINOR__) && 2093 <= (__GNUC__ * 1000 + __GNUC_MINOR__)
  /* but it's harmful in C++ code 
   * (gcc 3.2.1 complained about erroneous yylerrsp declaration) */
#  ifndef __cplusplus
  __attribute__ ((__unused__))
#  endif
#endif


  goto yyerrlab2;


/*---------------------------------------------------------------.
| yyerrlab2 -- pop states until the error token can be shifted.  |
`---------------------------------------------------------------*/
yyerrlab2:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}



