#ifndef QUERY_LEXER_H
#define QUERY_LEXER_H

#ifndef YY_DECL

#define YY_DECL                                         \
    int QParser::QueryLexer::lex(                       \
        QParser::QueryParserGen::semantic_type* yylval  \
    )

#endif

#ifndef __FLEX_LEXER_H
#define yyFlexLexer QueryFlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#endif



#include "parser.tab.hpp"

namespace QParser{
    class QueryLexer : public QueryFlexLexer {
	public:
	QueryLexer(std::istream* arg_yyin = 0, std::ostream* arg_yyout = 0);
	virtual ~QueryLexer();
	virtual int lex(QueryParserGen::semantic_type* yylval);
    };
}

#endif /* QUERY_LEXER_H */
