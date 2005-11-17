#include <iostream>
#include <string>
#include <sstream>
#include <FlexLexer.h>
#include "TreeNode.h"
#include "QueryParser.h"

yyFlexLexer* lexer;
extern QParser::TreeNode *d;
int yyparse();

using namespace std;

namespace QParser {

/*	(MH: replaced this old header...)
	TreeNode* QueryParser::parse(string query)
*/
	int QueryParser::parseIt(string query, TreeNode *&qTree)
	{
		stringstream ss (stringstream::in | stringstream::out);
		ss << query;
		lexer = new yyFlexLexer(&ss); 
		yyparse();
		delete lexer;
		qTree = d;
		return 0;
/*		return d;
*/
	}  
}
