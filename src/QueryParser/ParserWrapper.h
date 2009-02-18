#ifndef QUERY_PARSER_WRAPPER_H
#define QUERY_PARSER_WRAPPER_H

#include <memory>
#include <QueryParser/QueryLexer.h>
#include <QueryParser/TreeNode.h>

namespace QParser {
#include <QueryParser/parser.h>
	int yylex(YYSTYPE *lvalp, QueryLexer *lexer);
	void yyerror(QueryLexer*, TreeNode **, const char *);

	class ParserWrapper {
		private:
			mutable QueryLexer lexer;
			
		public:
			std::auto_ptr<TreeNode> parse(const
					std::string &stmt) const;
	};
}

#endif /* ADMIN_PARSER_H */
