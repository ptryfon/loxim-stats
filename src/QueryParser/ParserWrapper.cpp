#include <memory>
#include <sstream>
#include <QueryParser/ParserWrapper.h>
#include <QueryParser/QueryLexer.h>
#include <QueryParser/TreeNode.h>
#include <Errors/Errors.h>
#include <Errors/Exceptions.h>
#include <QueryParser/IndexNode.h>

using namespace std;
using namespace Errors;

using namespace Indexes;

namespace QParser{
	class QueryLexer;
#include <QueryParser/parser.h>
	int yylex(YYSTYPE *lvalp, QueryLexer *lexer) 
	{
		return lexer->lex(lvalp);
	}
	void yyerror(QueryLexer*, TreeNode **, const char *)
	{
	}

#include <QueryParser/parser.c>

	auto_ptr<TreeNode> ParserWrapper::parse(const std::string &stmt) const
	{
		stringstream ss(stringstream::in | stringstream::out);
		ss << stmt;
		lexer.switch_streams(&ss, 0);
		TreeNode *node;
		int res = yyparse(&lexer, &node);
		if (res){
			throw LoximException(ENotParsed);
		}
		return auto_ptr<TreeNode>(node);
	}
}
