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

	int QueryParser::parseIt(string query, TreeNode *&qTree)
	{
		stringstream ss (stringstream::in | stringstream::out);
//		string querytest = "pracownik ;";
		ss << query;
		lexer = new yyFlexLexer(&ss); 
		yyparse();
		printf( "po tescie d: %d. \n", d);
		delete lexer;
		qTree = d;
		return 0;
	}  
}
/*
int main () {


    QParser::QueryParser *qp = new QParser::QueryParser();
    QParser::TreeNode *tNode;
    qp->parseIt("cokolwiek ;", tNode);

}
*/