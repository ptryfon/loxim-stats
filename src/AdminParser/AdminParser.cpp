#include <memory>
#include <sstream>
#include <AdminParser/AdminParser.h>
#include <AdminParser/AdminExecutableTreeNode.h>
#include <AdminParser/AdminTreeNode.h>
#include <AdminParser/AdminTreeValueNode.h>
#include <AdminParser/ShutdownTreeNode.h>
#include <AdminParser/StatsControlNode.h>
#include <Errors/Errors.h>
#include <Errors/Exceptions.h>
#include <Errors/ErrorConsole.h>

using namespace std;
using namespace Errors;

namespace AdminParser{
	class AdminLexer;
	class AdminTreeNode;
#include <AdminParser/parser.h>
	int yylex(YYSTYPE *lvalp, AdminLexer *lexer) 
	{
		//YYSTYPE *node = NULL;
		return lexer->lex(lvalp);
	}
	void yyerror(AdminLexer*, AdminExecutableTreeNode **, const char *)
	{
	}

#include <AdminParser/parser.c>

	auto_ptr<AdminExecutableTreeNode> AdminParser::parse(const std::string &stmt) const
	{
		stringstream ss(stringstream::in | stringstream::out);
		ss << stmt;
		lexer.switch_streams(&ss, 0);
		AdminExecutableTreeNode *node;
		int res = yyparse(&lexer, &node);
		if (res){
			throw LoximException(ENotParsed);
		}
		return auto_ptr<AdminExecutableTreeNode>(node);
	}
}
