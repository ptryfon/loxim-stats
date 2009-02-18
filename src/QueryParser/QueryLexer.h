#ifndef QUERY_LEXER_H
#define QUERY_LEXER_H

#ifndef YY_DECL

#define YY_DECL              \
	int QueryLexer::lex( \
        YYSTYPE* yylval     \
    )

#endif

#ifndef __FLEX_LEXER_H
#define yyFlexLexer QueryFlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#endif

#include <iostream>

namespace QParser{
	union YYSTYPE;
	class QueryLexer : public QueryFlexLexer {
		public:
			QueryLexer(std::istream* arg_yyin = 0, std::ostream*
					arg_yyout = 0);
			virtual ~QueryLexer();
			virtual int lex(YYSTYPE* yylval);
	};
}

#undef __FLEX_LEXER_H

#endif /* QUERY_LEXER_H */
