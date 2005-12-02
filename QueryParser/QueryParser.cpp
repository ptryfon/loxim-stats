#include <iostream>
#include <string>
#include <sstream>
#include <FlexLexer.h>
#include "TreeNode.h"
#include "QueryParser.h"
#include "../Errors/ErrorConsole.h"
#include "../Errors/Errors.h"


yyFlexLexer* lexer;
extern QParser::TreeNode *d;
int yyparse();

// using namespace std;
 using namespace Errors;	// FIXME 

namespace QParser {

	int QueryParser::parseIt(string query, TreeNode *&qTree)
	{
		ErrorConsole *ec = new ErrorConsole("Parser");
		ec->init(1);
		
		stringstream ss (stringstream::in | stringstream::out);
		ss << query;
		lexer = new yyFlexLexer(&ss); 
	        int res = yyparse();
		if (res != 0){
		    *ec << "zapytanie nie sparsowane robi return...\n";
		    *ec << (ErrQParser | 5);	// zglosic jakis blad do Bagginsa ENotParsed ...    
		    return (ErrQParser | 5);
		} else
		    *ec << "zapytanie sparsowane chyba ok\n";
		delete lexer;
		qTree = d;
		
		*ec << "po parsowaniu treeNode: ";
		*ec << ((int) qTree);
		*ec << "Odczyt z drzewka, ktore przekazuje: \n";
		*ec << "----------------------------------------\n";
		qTree->putToString();		// putToString uzywa coutow i dlatego inna kolejnosc wypisywania FIXME
		*ec << "\n----------------------------------------\n";		
		//printf( "po parsowaniu treeNode: %d. \n", qTree);
		//cout << "88Odczyt z drzewka, ktore przekazuje:" << endl;
		//cout << "88--------------------------------------" << endl;
		//qTree->putToString();
		//cout << "\n88--------------------------------------" << endl;
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

