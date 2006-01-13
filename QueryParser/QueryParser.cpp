#include <iostream>
#include <string>
#include <sstream>
#include <FlexLexer.h>
#include "TreeNode.h"
#include "Stack.h"
#include "QueryParser.h"
//#include "../Errors/ErrorConsole.h"
//#include "../Errors/Errors.h"


yyFlexLexer* lexer;
extern QParser::TreeNode *d;
int yyparse();

// using namespace std;
//using namespace Errors;	

namespace QParser {

	int QueryParser::parseIt(string query, TreeNode *&qTree)
	{
//		ErrorConsole *ec = new ErrorConsole("Parser");
		// ec->init(1);
//		cerr << "stworzyl errConsole \n";
//		*ec << "PARSER::parseIt start\n";
//		cerr << "uzyl errCons\n";
				
		stringstream ss (stringstream::in | stringstream::out);
		ss << query;
		lexer = new yyFlexLexer(&ss); 
	        int res = yyparse();
		if (res != 0){
//		    *ec << "zapytanie nie sparsowane robi return...\n";
//		    *ec << (ErrQParser | ENotParsed);	 
		    cout << "zapytanie nie sparsowane robi return .. \n";
		    return -1;		    
//		    return (ErrQParser | ENotParsed);
		} else
//			    *ec << "zapytanie sparsowane chyba ok\n";
		    cout << "zapuytanie sparsowane chyba ok \n";
		delete lexer;
		qTree = d;
		

		printf( "po parsowaniu treeNode: %d. \n", qTree);
		cout << "Odczyt z drzewka, ktore przekazuje:" << endl;
		cout << "--------------------------------------" << endl;
		qTree->putToString();
		cout << "\n--------------------------------------" << endl;
//		*ec << "PARSER::parseIt end\n";
		cout << "koniec parseIt\n";

		return 0;
	}  

}

/*
int main (int argc, char* argv[]) {

    string theStr = argv[1];
    
    cout << "zaraz zaczne maina \n";
	cout << "dostalem stringa z parametru: " << theStr <<"\n" ;
    QParser::QueryParser *qp = new QParser::QueryParser();
    QParser::TreeNode *tNode;
    qp->parseIt(theStr, tNode);
    
//    cout << "zaraz zrobie factorSub" << endl;
//    QParser::TreeNode *modNode = tNode->factorSubQuery( tNode->getRArg()->getRArg(), "sik");
//    cout << "\n ~~~~~~~~~~~~~~~~~~" << endl;
//    modNode->putToString();
//    cout << "\n ~~~~~~~~~~~~~~~~~~" << endl;    
    cout << "skonczylem maina \n";
    return 0;
}
*/





