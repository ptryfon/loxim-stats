#ifndef ADMIN_LEXER_H
#define ADMIN_LEXER_H

#ifndef YY_DECL

#define	YY_DECL							\
	int ::AdminParser::AdminLexer::lex(				\
	AdminTreeNode* yylval		\
    )
#endif

#ifndef __FLEX_LEXER_H
#define yyFlexLexer AdminFlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#endif

#include <AdminParser/AdminParser.h>

namespace AdminParser {

	class AdminTreeNode;

	class AdminLexer : public AdminFlexLexer
	{
		public:
			AdminLexer(std::istream* arg_yyin = 0,
					std::ostream* arg_yyout = 0);

			virtual ~AdminLexer();

			virtual int lex(AdminTreeNode* yylval);

	};

} 

#endif /* ADMIN_LEXER_H */