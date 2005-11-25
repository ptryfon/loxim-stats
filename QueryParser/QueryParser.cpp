#include <iostream>
#include <string>
#include <sstream>
#include <FlexLexer.h>
#include "TreeNode.h"
#include "QueryParser.h"

yyFlexLexer* lexer;
extern QParser::TreeNode *d;
int yyparse(void);

using namespace std;

namespace QParser {

	int QueryParser::parseIt(string query, TreeNode *&qTree)
	{
		stringstream ss (stringstream::in | stringstream::out);
		stringstream ss2 (stringstream::in | stringstream::out);
		string completeQuery = query+" ;";
		ss << query;
		lexer = new yyFlexLexer(&ss); 
		int res = yyparse();
		if (res != 0) {
		
		    cout << " \n\nDODAWAC SREDNIKI NA KONCU ZAPYTAN !\n\n " << endl;
		    cout << "Twoje zapytanie zostalo niepoprawnie sparsowane.." << endl;
		    cout << "... kto wie, moze brakuje srednika na koncu?" << endl;
		    cout << "Doklejam srednik i sprobuje jeszcze raz..." << endl;
		    cout << "hm... " << endl;
		    ss2 << completeQuery;
		    delete lexer;
		    lexer = new yyFlexLexer(&ss2);
		    int res = yyparse();
		    if (res != 0) cout << "Niestety! To nie byla kwestia srednika :("<< endl;		    
		    else cout << "Ze srednikiem na koncu to jest poprawne zapytanie! "<< endl;
		} else
		    cout << "zapytanie jest chyba OK" << endl;
		if (d == NULL) 
		    cout << "Wynikowe drzewko TreeNode jest puste !!" << endl;
		else cout << "Wynikowe TreeNode NIE jest puste ! " << endl;
		delete lexer;
		qTree = d;
		return 0;
	}  
}

/*
int main () {


    QParser::QueryParser *qp = new QParser::QueryParser();
    QParser::TreeNode *tNode;
    qp->parseIt("cokolwiek", tNode);

}
*/
