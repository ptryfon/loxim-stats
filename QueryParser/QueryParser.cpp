#include <iostream>
#include <string>
#include <sstream>
#include <FlexLexer.h>
#include "TreeNode.h"
#include "Stack.h"
#include "DataRead.h"
#include "QueryParser.h"
//#include "../Errors/ErrorConsole.h"
//#include "../Errors/Errors.h"
#include "../Config/SBQLConfig.h"

yyFlexLexer* lexer;
extern QParser::TreeNode *d;
int yyparse();

// using namespace std;
//using namespace Errors;	
using namespace Config;
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

//			QParser::Optimiser *opt = new QParser::Optimiser();
//	int reslt;
/*	cout << "now the following tree will be evaluated statically.." << endl;
	TreeNode *nt = d->clone();
	nt->putToString();
	if ((reslt = opt->stEvalTest(d)) != 0)
	    fprintf (stderr, "static evaluation did not work out...\n");
	else {
	    cout << "static evaluatioin OK, result: "<< reslt << endl;
*/	    /*tu bedzie optymalizacja... */    
/*	    	fprintf (stderr, "now I will try to optimise the tree..\n");
		int optres = nt->optimizeTree();
		fprintf (stderr, "__%d__", optres);

		fprintf (stderr, "end of optimisation.\n");
	}

*/
	
	int optres = -2;
	int stat_ev_res;
	QParser::Optimiser *opt = new QParser::Optimiser();
	TreeNode *nt = d->clone();
	
//	/* The main optimisation loop - factor out single independent subqueries *
//	 * as long as ... such exist !                                           */
//	while (optres == -2) {
	bool shouldOptimize;
	SBQLConfig conf("QueryParser");
	conf.getBool("optimisation", shouldOptimize);
	if ( shouldOptimize	) {
		fprintf (stderr, " optimisation is set ON !\n");
		if ((stat_ev_res = opt->stEvalTest(nt)) != 0) {
		fprintf (stderr, "static evaluation did not work out ... \n");
		optres == -1;
	    } else {
		optres = nt->optimizeTree();
		while (nt->getParent() != NULL) nt = nt->getParent();
		}
	} else fprintf (stderr, " optimisation is set OFF !\n");
			
//	
//	    }
//	}
	
	if (optres != -1) {fprintf(stderr, "I'll return optimized tree\n"); qTree = nt;}
	else { fprintf (stderr, "I'll return the tree from before static eval..\n");}
//	

	string zap = "EMP where SAL = (EMP where NAME=\"KUBA\").SAL;";
	
	if (false && (query == zap)){
	    cout << "TEST START---------------------------------------------------------------------------------" << endl;
	    stringstream ss (stringstream::in | stringstream::out);
	    ss << zap;
	    lexer = new yyFlexLexer(&ss); 
	    int res = yyparse();
	
	    delete lexer;
	    TreeNode *tree = d;

	    printf( "po parsowaniu treeNode: %d. \n", tree);
	    cout << "Odczyt z drzewka, ktore przekazuje:" << endl;
	    cout << "--------------------------------------" << endl;
	    tree->putToString();
	    cout << "\n--------------------------------------" << endl;
    	    QParser::Optimiser *opt = new QParser::Optimiser();
	    int reslt;
	    cout << "now the following tree will be evaluated statically.." << endl;
	    TreeNode *nt = d->clone();

	    if ((reslt = opt->stEvalTest(nt)) != 0)
		fprintf (stderr, "static evaluation did not work out...\n");
	    else {
		cout << "static evaluatioin OK, result: "<< reslt << endl;
	    
    
		fprintf (stderr, "now I will try to optimise the tree..\n");

		cout << "OPTYMALIZUJE--------------------------------------------------------"<< endl;
		//cout << "przed optymalizacjia" << endl;
		//nt->putToString();
		//cout << "-----------" << endl;
		
		optres = nt->optimizeTree();
		cout << "KONIEC OPTYMALIZACJI-------------------------------------------------------"<< endl;
		/*the nt tree needs to be 'rolled' up: (in case the nonalgopnode
		  we were operating on was the root... */
		while (nt->getParent() != NULL) nt = nt->getParent();

		fprintf (stderr, "__%d__", optres);

		cout << "po JEDNYM PRZEBIEGU optymalizacji" << endl;
		nt->putToString();
		cout << "-----------" << endl;
		fprintf (stderr, "end of optimisation.\n");
	    }
	    cout << "TEST END__---------------------------------------------------------------------------------" << endl;
	}

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
    
//	cout << "teraz bede testowal stosy... " << endl;
//	QParser::Optimiser *opt = new QParser::Optimiser();
//	int res = opt->simpleTest();

//	cout << "to tyle z testow stosow. wynik testow: "<< res << endl;
//    cout << "zaraz zrobie factorSub" << endl;
//    QParser::TreeNode *modNode = tNode->factorSubQuery( tNode->getRArg()->getRArg(), "sik");
//    cout << "\n ~~~~~~~~~~~~~~~~~~" << endl;
//    modNode->putToString();
//    cout << "\n ~~~~~~~~~~~~~~~~~~" << endl;    
  //  cout << "skonczylem maina \n";
    return 0;
}

*/






