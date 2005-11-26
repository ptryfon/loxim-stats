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
		ss << query;
		lexer = new yyFlexLexer(&ss); 
	        int res = yyparse();
		if (res != 0)
		    cout << "zapytanie nie sparsowane..." << endl;
		else
		    cout << "zapytanie sparsowane chyba ok" << endl;
		delete lexer;
		qTree = d;
		printf( "po parsowaniu treeNode: %d. \n", qTree);
		cout << "Odczyt z drzewka, ktore przekazuje:" << endl;
		cout << "--------------------------------------" << endl;
		qTree->putToString();
		cout << "\n--------------------------------------" << endl;
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

