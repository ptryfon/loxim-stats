#include <iostream>
#include <string>
#include <sstream>
#include <FlexLexer.h>
#include "TreeNode.h"
#include "Stack.h"
#include "DataRead.h"
#include "QueryParser.h"
#include "DeathRmver.h"
#include "AuxRmver.h"
#include "JoinRpcer.h"
#include "../Errors/ErrorConsole.h"
#include "../Errors/Errors.h"
#include "../Config/SBQLConfig.h"
#include "Deb.h"
yyFlexLexer* lexer;
extern QParser::TreeNode *d;
int yyparse();

// using namespace std;
using namespace Errors;	
using namespace Config;

namespace QParser {
	
	int  QueryParser::statEvalRun = 0;
	int  QueryParser::getStatEvalRun(){return statEvalRun;};
	void QueryParser::setStatEvalRun(int n){QueryParser::statEvalRun = n;};
	void QueryParser::incStatEvalRun(){statEvalRun++;};
    void QueryParser::setQres(StatQResStack *nq) {this->sQres = nq;}
    void QueryParser::setEnvs(StatEnvStack *nq) {this->sEnvs = nq;}
    QueryParser::~QueryParser() {if (sQres != NULL) delete sQres;
				if (sEnvs != NULL) delete sEnvs;}
	QueryParser::QueryParser() {
		sQres = NULL; 
		sEnvs = NULL; 
		SBQLConfig conf("QueryParser");
		conf.getBool("optimisation", shouldOptimize);
		if (shouldOptimize) {
			DataScheme::reloadDScheme();
		}
	}
	
    int QueryParser::statEvaluate(TreeNode *&tn) {
    	QueryParser::incStatEvalRun();
	/* init the both static stacks. The data model is available as	 *
	 * DataRead::dScheme()						 */
	setEnvs(new StatEnvStack());
	setQres(new StatQResStack());
		
	/* set the first section of the ENVS stack with binders to *
	 * definitions of base (i.e. root) objects ...             */
    	sEnvs->pushBinders(DataScheme::dScheme()->bindBaseObjects());	
	if (Deb::ugOn()) {
	    sEnvs->putToString();
	    cout << endl;
	}	
	Deb::ug("OK, the ENVS is ready with root objects on the bottom.");
	/* evoke static_eval, which will fill in special info in nameNodes and NonAlgOpNodes */
	int res = tn->staticEval (sQres, sEnvs);
	if (Deb::ugOn()) {tn->putToString(); cout << endl; }
	
	return res;	
    }


    int QueryParser::parseIt(string query, TreeNode *&qTree) {
    	QueryParser::setStatEvalRun(0);
	ErrorConsole ec("QueryParser");
	
	/* check if this is debug mode or not... */	
	Deb::setWriteDebugs();		
	Deb::ug("debug mode is ON! you can change it in the configuration file");
	stringstream ss (stringstream::in | stringstream::out);
	ss << query;
	lexer = new yyFlexLexer(&ss); 
	int res = yyparse();
	if (res != 0){
    	    ec << "zapytanie nie sparsowane robi return...\n";
	    ec << (ErrQParser | ENotParsed);	 
    	    return (ErrQParser | ENotParsed);
//	Deb::ug("zapytanie nie sparsowane robi return .. \n");
//	return -1;		    
	} else {
//			    *ec << "zapytanie sparsowane chyba ok\n";
	    Deb::ug("zapuytanie sparsowane chyba ok ");
	}	
	delete lexer;
	qTree = d;
	
	Deb::ug("Odczyt z drzewka, ktore przekazuje:");
	Deb::ug("--------------------------------------");
	if (Deb::ugOn()) {
	    qTree->putToString(); cout << endl;
	}
	Deb::ug("--------------------------------------");

	int stat_ev_res;
	TreeNode *nt = d->clone();

	
	
	if (shouldOptimize) {
	    Deb::ug(" optimisation is set ON !");
	    int optres = 0;
	    if ((stat_ev_res = this->statEvaluate(nt)) != 0) {
		Deb::ug("static evaluation did not work out ...");
		optres = -1;
	    } else {
    		int optres = -2;
    		DeathRmver *rmver = new DeathRmver(this);
    		rmver->rmvDeath(nt);
    		if (this->statEvaluate(nt) != 0) optres = -1;
		/* The main optimisation loop - factor out single independent subqueries *
		 * as long as ... such exist !                                           */
    		while (optres == -2) {
    		    optres = nt->optimizeTree();
    		    while (nt->getParent() != NULL) nt = nt->getParent();
    		    /*one more static eval, to make sure nodes have the right info.. */
    		    fprintf (stderr, "one more stat eval..\n");
	    	    if (this->statEvaluate(nt) != 0) optres = -1;
		}
		AuxRmver *auxRmver = new AuxRmver(this);
		auxRmver->rmvAux(nt);
		JoinRpcer *joinRpcer = new JoinRpcer(this);
		joinRpcer->replaceJoin(nt);
	    }
	    if (optres != -1) {
		Deb::ug("I'll return optimized tree\n"); 
		qTree = nt;
		
		Deb::ug("Odczyt z drzewka, po zoptymalizowaniu:");
		Deb::ug("--------------------------------------");
		if (Deb::ugOn()) {
	        qTree->putToString(); cout << endl;
		Deb::ug("--------------------------------------");
		}
	    } else { Deb::ug("I'll return the tree from before static eval..");}
	} else Deb::ug(" optimisation is set OFF !\n");

	Deb::ug(" ParseIt() ends successfully!");
	/*
	cout << "td1------------------------------------------------------------\n";
	testDeath("EMP.NAME");
	cout << "td2------------------------------------------------------------\n";
	testDeath("(EMP join (WORKS_IN.DEPT)).NAME;");
	cout << "td3------------------------------------------------------------\n";
	*/
	
	cout <<"=============================================================================" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout<< "P    A    R    S    E    R        Z    W    R    A    C    A    :" << endl;
//	qTree->putToString();
	qTree->serialize();
	cout << endl;
	return 0;
    }
    
    void QueryParser::testDeath(string _zap){
//    	string zap = "(EMP join (WORKS_IN.DEPT)).NAME;";  
	    cout << "TESTDEATH START---------------------------------------------------------------------------------" << endl;
	    stringstream ss (stringstream::in | stringstream::out);
	    ss << _zap;
	    lexer = new yyFlexLexer(&ss); 
	    int res = yyparse();
	
		cout << "parse result: " << res << endl;
	    delete lexer;
	    TreeNode *tree = d;

	    printf( "po parsowaniu treeNode: %d. \n", (int) tree);
	    cout << "Odczyt z drzewka, ktore przekazuje:" << endl;
	    cout << "--------------------------------------" << endl;
	    tree->putToString();
	    cout << "\n--------------------------------------" << endl;
    	    QParser::DeathRmver *rmver = new QParser::DeathRmver(this);
	    int reslt;
	    cout << "now the following tree will be evaluated statically.." << endl;
	    TreeNode *nt = d->clone();

		rmver->rmvDeath(nt);
		
	    cout << "TEST END__---------------------------------------------------------------------------------" << endl;
		cout << "koniec testDeath\n";
    }

    int QueryParser::testParse (string query, TreeNode *&qTree) {
	string zap = "EMP where SAL = (EMP where NAME=\"KUBA\").SAL;";
	
	if (false && (query == zap)){
	    cout << "TEST START---------------------------------------------------------------------------------" << endl;
	    stringstream ss (stringstream::in | stringstream::out);
	    ss << zap;
	    lexer = new yyFlexLexer(&ss); 
	    int res = yyparse();
	
	cout << "parse result: " << res << endl;
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
		
		int ooptres = nt->optimizeTree();
		cout << "KONIEC OPTYMALIZACJI-------------------------------------------------------"<< endl;
		/*the nt tree needs to be 'rolled' up: (in case the nonalgopnode
		  we were operating on was the root... */
		while (nt->getParent() != NULL) nt = nt->getParent();

		fprintf (stderr, "__%d__", ooptres);

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





