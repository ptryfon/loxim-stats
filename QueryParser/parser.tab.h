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




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 11 "parser.y"
typedef union YYSTYPE {
  char* str;
  int num;
  double dbl;
  QParser::TreeNode* tree;
  QParser::QueryNode* qtree;
} YYSTYPE;
/* Line 1252 of yacc.c.  */
#line 152 "parser.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



