#ifndef ADMIN_PARSER_H
#define ADMIN_PARSER_H

#include <memory>
#include <AdminParser/AdminLexer.h>
#include <SystemStats/StatsTypes.h>

namespace AdminParser {
	class AdminLexer;
	class AdminExecutableTreeNode;
	class AdminTreeNode;
#include <AdminParser/parser.h>
	int yylex(YYSTYPE *lvalp, AdminLexer *lexer);
	void yyerror(AdminLexer*, AdminExecutableTreeNode **, const char *);

	class AdminParser {
		private:
			mutable AdminLexer lexer;
			
		public:
			std::auto_ptr<AdminExecutableTreeNode> parse(const
					std::string &stmt) const;
	};
}

#endif /* ADMIN_PARSER_H */
