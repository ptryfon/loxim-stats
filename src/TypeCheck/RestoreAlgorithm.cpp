
#include "RestoreAlgorithm.h"
#include "QueryParser/Deb.h"

#include <string>
//#include <fstream>
#include <iostream>
#include <sstream>
//#include <map>
#include <vector>

using namespace QParser;
using namespace std;


namespace TypeCheck {

	DTCRestoreAlgorithm::DTCRestoreAlgorithm(){sQres = NULL; sEnvs = NULL;};
	
	DTCRestoreAlgorithm::DTCRestoreAlgorithm(StatQResStack *sq, StatEnvStack *se) {
		//assume these are well initialized by TypeChecker.
		sQres = sq;
		sEnvs = se;
	};

	string DTCRestoreParam::toString() {
		stringstream ss;
		ss << "(";
		ss << param;
		ss << ",";
		ss << option;
		ss << ")";
		return ss.str();
	}
	
	//return (ErrTypeChecker | ECannotRestore); - if you cannot restore. 
	int DTCRestoreAlgorithm::restoreOnBadArg(TreeNode *tn, int tType, int op, TypeCheckResult &tc, Signature *l, Signature *r, RestoreParam *rp){
		
		if (l == NULL || r == NULL) return (ErrTypeChecker | ETCInnerFailure);		
		int errcode = 0;
		TypeCheckResult tcRes;
		if (Deb::ugOn()) cout << "CALLING RestoreOnBadArg ( 2 - args ) " << endl;
		RDecisionTable *rdTab = RestoreDecisionTableHandler::getHandle()->getDTable(tType, op);
		errcode = rdTab->getResult(tcRes, l, r);
		if (errcode != 0) return errcode;
		if (Deb::ugOn()) cout << "[RESTORE] '" << op << "' got such tcresult: \n" << tcRes.printAllInfo() << endl;
		if (tcRes.isError() || tcRes.getSig() == NULL) {//Restore failed - return with final error.
			return (ErrTypeChecker | ECannotRestore);
		}
		//On success...
		tcRes.fill(tc);
		return 0;
	}
	
	int DTCRestoreAlgorithm::restoreOnBadArg(TreeNode *tn, int tType, int op, TypeCheckResult &tc, Signature *sig, RestoreParam *rp) {
		
		if (sig == NULL) return (ErrTypeChecker | ETCInnerFailure);
		int errcode = 0;		
		TypeCheckResult tcRes;
		if (Deb::ugOn()) cout << "CALLING RestoreOnBadArg ( UNOP ) " << endl;		
		RUnOpDecisionTable *ruDTab = RestoreDecisionTableHandler::getHandle()->getUnOpDTable(tType, op);
		errcode = ruDTab->getResult(tcRes, sig, ((DTCRestoreParam *)rp)->param, ((DTCRestoreParam *)rp)->option);
		if (errcode != 0) return errcode;
		if (Deb::ugOn()) cout << "UNOP(" << op << "), got such tcresult: \n" << tcRes.printAllInfo() << endl;
		if (tcRes.isError() || tcRes.getSig() == NULL) { // Restore failed - return final error.
			return (ErrTypeChecker | ECannotRestore);
		}
		tcRes.fill(tc);
		return 0;
	}
	
	int DTCRestoreAlgorithm::restoreOnMisspelledName(TreeNode *tn, string name, vector<BinderWrap*> *vec) {
		if (Deb::ugOn()) cout << "Would call: RestoreAfterMisspelledName ! for name: " << name << endl;
		//go through the stack and decide which name is 'the closest' to name. If none is really close, 
		// return ECannotRestoreBadName. Else - fill the vector in and return 0;
		return (ErrTypeChecker | ECannotRestoreBadName); //For now - lets presume we can never guess the right name. 
	}

	
	/* ****************	  RestoreDecisionTableHandler methods	******************** */	
	
	RestoreDecisionTableHandler *RestoreDecisionTableHandler::rdTableHandler = new RestoreDecisionTableHandler();
	RestoreDecisionTableHandler *RestoreDecisionTableHandler::getHandle() {
		return rdTableHandler;
	}
		
	RDecisionTable::RDecisionTable(int opType, int oper) {create(opType, oper);}
	//can't do that thing below, the contsr. arent virtual
	//RDecisionTable::RDecisionTable(int treeType, int oper) : DecisionTable(treeType, oper) {}
	
	RUnOpDecisionTable::RUnOpDecisionTable(int treeType, int oper) {create(treeType, oper);}
	
	
	RDecisionTable *RestoreDecisionTableHandler::getDTable(int algOrNonAlg, int op) {
		map<int, RDecisionTable*> *tableMap;
		int place = op;
		switch (algOrNonAlg) {
			case TreeNode::TNALGOP : tableMap = &algOpRestoreDTables; break;
			case TreeNode::TNNONALGOP : tableMap = &nonAlgOpRestoreDTables; break;
			//a new tree node with many operators may need new tableMap
			default : tableMap = &otherBinaryRestoreDTables; place = (_TN_MAX_OP * algOrNonAlg) + op; break;
		}
		if ((*tableMap).find(place) == (*tableMap).end()) {
			(*tableMap)[place] = new RDecisionTable(algOrNonAlg, op);
		}
		return (*tableMap)[place];
	}	
	
	RUnOpDecisionTable *RestoreDecisionTableHandler::getUnOpDTable(int treeType, int op) {
		map<int, RUnOpDecisionTable*> *tableMap;
		int place = op;
		cout << "place: " << place << endl;
		if (treeType == TreeNode::TNUNOP) tableMap = &unOpRestoreDTables;
		else { 
			tableMap = &otherUnaryRestoreDTables;
			cout << "place changed: " << place << endl;
			place = (_TN_MAX_OP * treeType) + op;
		}
		
		if ((*tableMap).find(place) == (*tableMap).end()) {
			cout << "creating unopDTable(" << treeType << ", " << op << ") at " << place << "\n";
			(*tableMap)[place] = new RUnOpDecisionTable(treeType, op);
		}
		return (*tableMap)[place];
	}
	
	RestoreDecisionTableHandler::~RestoreDecisionTableHandler() {
		unOpRestoreDTables.clear();
		algOpRestoreDTables.clear();
		nonAlgOpRestoreDTables.clear();
		otherBinaryRestoreDTables.clear();
		otherUnaryRestoreDTables.clear();
	}	
	
	/* ****************	  end of RestoreDecisionTableHandler methods	******************** */	
	
	
	/* **************** Restore dec. table rules	********************** */
	
	
	void RDecisionTable::initAlgOpRules(int op) {
		//cout << "[TC]: " << "inits Restore AlgOpRules: " << op << " operator." << endl;
		switch (op) {
			case AlgOpNode::bagIntersect: {
				addTcRule(M_B,		"X",		"X",		BS_COPY_L);
				/*CARD*/		
				addCdRule(M_B,		"X",		"Y",		CD_INTST);
				/*T-NAME*/		
				addTnRule(M_B,		"X",		"X",		"");
				/*C-KIND*/		
				addCkRule(M_B,		"any",		"any",		"bag");
				break;
			}
			case AlgOpNode::bagMinus: {
				addTcRule(M_B,		"X",		"X",		BS_COPY_L);
				/*CARD*/		
				addCdRule(M_B,		"X",		"Y",		CD_COPY_L_ZR);
				/*T-NAME*/		
				addTnRule(M_B,		"X",		"X",		"");
				/*C-KIND*/		
				addCkRule(M_B,		"any",		"any",		"bag");
				break;
			}
			case AlgOpNode::bagUnion: { 
				/*BASE*/	
				addTcRule(M_B,		"X",		"X",		BS_COPY_L);
				/*CARD*/		
				addCdRule(M_B,		"X",		"Y",		CD_ADDBANDS);
				/*T-NAME*/		
				addTnRule(M_B,		"X",		"X",		"");
				/*C-KIND*/		
				addCkRule(M_B,		"any",		"any",		"bag");
				break;
			}
			case AlgOpNode::plus: 
			case AlgOpNode::minus: {
				/*BASE*/
				addTcRule(M_EX,		"integer",	"X",		"integer");
				addTcRule(M_EX,		"double",	"X",		"double");
				addTcRule(M_EX,		"string",	"X",		"string");
				addTcRule(M_EX,		"struct",	"X",		TC_RS_ERROR); //asked to subtr. structs-dont even guess.
				addTcRule(M_ELSE, 							"double");	//most often result in primary dtable...
				/*CARD*/		
				addCdRule(M_B,		"x..y",		"x..y",		"1..1");
				/*T-NAME*/		
				addTnRule(M_B,		"x",		"y",		"");
				break;			
			}
			case AlgOpNode::times: 
			case AlgOpNode::divide: { 
				addTcRule(M_EX,		"integer",	"X",		"integer");
				addTcRule(M_EX,		"double",	"X",		"double");
				addTcRule(M_EX,		"string",	"string",	"string");
				addTcRule(M_EX,		"struct",	"X",		TC_RS_ERROR);
				addTcRule(M_ELSE, 							"double");	//most often result in primary dtable...
				/*CARD*/		
				addCdRule(M_B,		"x..y",		"x..y",		"1..1");
				/*T-NAME*/		
				addTnRule(M_B,		"x",		"y",		"");
				break;
			}
			case AlgOpNode::eq: 
			case AlgOpNode::neq: 
			case AlgOpNode::lt: 
			case AlgOpNode::gt:
			case AlgOpNode::le:
			case AlgOpNode::ge:
			case AlgOpNode::boolAnd:
			case AlgOpNode::boolOr: {
				/*BASE*/	
				addTcRule(M_B, 		"", 		"", 		"boolean");
				/*CARD*/		
				addCdRule(M_B,		"",			"",			"1..1");
				/*T-NAME*/		
				addTnRule(M_B,		"x",		"y",		"");
				break;
			}
			case AlgOpNode::comma: {
				/*BASE*/		
				addTcRule(M_B,		"X",		"Y",		BS_STRUCT);
				/*CARD*/		
				addCdRule(M_B,		"X",		"Y",		CD_MULTBANDS);
				/*T-NAME*/		
				addTnRule(M_B, 		"X", 		"Y", 		"");	
				/*C-KIND*/		
				addCkRule(M_B,		"X",		"Y", 		"bag");
				break;			
			} //structure constructor ... ??? 
			case AlgOpNode::assign: { // impertative - just ret. void
				addTcRule(M_B,		"X",		"Y",		"void");
				addCdRule(M_B,		"x..y",		"a..b",		"1..1");
				break;
			}	
			case AlgOpNode::insert: { 	// impertative - just ret. void
				addTcRule(M_B,		"X",		"Y",		"void");
				addCdRule(M_B,		"x..y",		"a..b",		"1..1");
				break;
			}
			case AlgOpNode::semicolon: { 
				addTcRule(M_B,		"X",		"Y",		BS_COPY_R);
				addCdRule(M_B,		"X",		"Y",		ARG_COPY_R);
				addTnRule(M_B,		"X",		"Y",		ARG_COPY_R);
				addCkRule(M_B,		"X",		"Y",		ARG_COPY_R);
				break;
			}
			case AlgOpNode::refeq: {
				/*BASE*/	
				addTcRule(M_B, 		"", 		"", 		"boolean");
				/*CARD*/		
				addCdRule(M_B,		"",			"",			"1..1");
				/*T-NAME*/		
				addTnRule(M_B,		"x",		"y",		"");
				break;
			}	
			default: break;
		}
		provideReturnLinks();
	}
	
	void RDecisionTable::initNonAlgOpRules(int op) {
		//cout << "[TC]: " << "inits Restore NonAlgOpRules: " << op << " operator." << endl;
		switch (op) {//TODO: fill the rulesets
			case NonAlgOpNode::dot: {
				/*BASE*/	
				addTcRule(M_B, 		"Y", 		"X", 		BS_COPY_R);
				/*CARD*/		
				addCdRule(M_B,		"X",		"Y",		CD_MULTBANDS);
				/*T-NAME*/		
				addTnRule(M_B,		"Y",		"X",		ARG_COPY_R);
				/*C-KIND*/		
				addCkRule(M_L, 		M_0,		"list",		"list");
				addCkRule(M_R,		"list",		M_0,		"list");
				addCkRule(			"list",		"list",		"list");
				addCkRule(M_ELSE,							"bag");	
				break;
			}
			case NonAlgOpNode::join: { //same rules as in struct, collKind taken from dot...
				/*BASE*/
				addTcRule(M_B,		"X",		"Y",		BS_STRUCT);
				/*CARD*/	
				addCdRule(M_B,		"X",		"Y",		CD_MULTBANDS);
				/*T-NAME*/	
				addTnRule(M_B,		"X",		"Y",		"");
				/*C-KIND*/	
				addCkRule(M_B,		"X",		"Y",		"bag");
				break;
			}
			case NonAlgOpNode::where: {
				/*BASE*/
				addTcRule(M_B, 		"X",		"Y",		BS_COPY_L);
				/*CARD*/	
				addCdRule(M_B,		"X..Y",		"Y",		CD_COPY_L_ZR);
				/*T-NAME*/	
				addTnRule(M_B,		"X",		"Y",		ARG_COPY_L);
				/*C-KIND*/	
				addCkRule(M_B,		"X",		"Y",		ARG_COPY_L);
				break;
			}
			case NonAlgOpNode::closeBy: 		// as BELOW..
			case NonAlgOpNode::closeUniqueBy: 	// as BELOW..
			case NonAlgOpNode::leavesBy: 		// as BELOW..
			case NonAlgOpNode::leavesUniqueBy: {
				/*BASE*/	
				addTcRule(M_B,		"X",		"X",		BS_COPY_L);
				/*CARD*/		
				addCdRule(M_B,		"X",		"Y",		CD_ADDBANDS);
				/*T-NAME*/		
				addTnRule(M_B,		"X",		"X",		"");
				/*C-KIND*/		
				addCkRule(M_B,		"any",		"any",		"bag");
				break;
			}
			case NonAlgOpNode::orderBy: { 
				addTcRule(M_B,		"X",		"Y",		BS_COPY_L);
				addCdRule(M_B,		"X",		"Y",		ARG_COPY_L);
				addTnRule(M_B,		"X", 		"Y", 		ARG_COPY_L);
				
				break;
			}
			case NonAlgOpNode::exists: //same as forAll..: BELOW
			case NonAlgOpNode::forAll: {
				/*BASE*/
				addTcRule(M_B,		"X",		"Y",		"boolean");
				/*CARD*/	
				addCdRule(M_B, 		"X..Y",		"a..b",		"1..1");
				break;
			}
			case NonAlgOpNode::forEach: {
				/*BASE*/	
				addTcRule(M_B,		"X",		"Y", 		"void");
				/*CARD*/		
				addCdRule(M_B,		"X",		"Y",		ARG_COPY_L);
				/*T-NAME*/		
				addTnRule(M_B,		"X",		"Y",		"");
				/*C-KIND*/		
				addCkRule(M_B,		"",			"", 		ARG_COPY_L);
				break;
			}
			default: break;
		}
		provideReturnLinks();
	}
	
	void RDecisionTable::initOtherBinaryRules(int treeType, int op) {
		switch (treeType) {
			case TreeNode::TNCASTTO : {
				/*BASE*/	
				addTcRule(M_B,		"Y",	"X",			BS_COPY_R);	//suppose cast was successful- we'd get rSig.
				/*CARD*/		
				addCdRule(M_B, 		"X", 		"Y", 		ARG_COPY_L);
				/*T-NAME*/	
				addTnRule(M_B,		"X",		"Y",		"");	
				/*C-KIND*/	
				addCkRule(M_B,		"X",		"Y",		ARG_COPY_L);
			}
			default: break;	
		}
		provideReturnLinks();
	}
	
	void RUnOpDecisionTable::initUnOpRules(int treeType, int op) {
		switch (treeType) {
		case TreeNode::TNUNOP :
			switch (op) {//TODO: fill the rulesets
			case UnOpNode::count: {
				addTcRule(META,		"x",		"integer");
				addCdRule(META,		"x",		"1..1");
				break;
			}
			case UnOpNode::sum: 
			case UnOpNode::avg:	
			case UnOpNode::min: 
			case UnOpNode::max: { 
				addTcRule("",		"integer",	"integer");
				addTcRule("",		"double",	"double");
				addTcRule("",		"string",	"string");
				addTcRule("",		"boolean",	"boolean");
				addTcRule(M_ELSE, 	"",			"double");
				/*CARD*/		
				addCdRule(META,		"x..y",			"1..1");
				/*T-NAME*/		
				addTnRule(META,		"X",		"");
				break;
			}
			case UnOpNode::deref:{	
				addTcRule("",		"string",	"string");
				addTcRule("",		"integer",	"integer");
				addTcRule("",		"double",	"double");
				addTcRule("",		"boolean",	"boolean");
				addTcRule(M_ELSE, 	"", 		BS_DEREF); 	
				addCdRule(META, 	"x", 		ARG_COPY);
				//addTnRule(META, 	"x", 		ARG_COPY);	
				addCkRule(META, 	"x", 		ARG_COPY);
				break;
			}
			case UnOpNode::ref: {	// copies argument, just setting the 'ref' flag.
				addTcRule(META,		"",			BS_COPY_REF);
				addCdRule(META, 	"x", 		ARG_COPY);
				addTnRule(META, 	"x", 		ARG_COPY);	
				addCkRule(META, 	"x", 		ARG_COPY);
				break;
			}
			case UnOpNode::distinct: {
				addTcRule(META,		"",			BS_COPY_REF);
				addCdRule(META, 	"x", 		ARG_COPY);		//nr of elts may change, but card stays as it is !
				addTnRule(META, 	"x", 		ARG_COPY);		//elements can keep their typename.
				addCkRule(META, 	M_0, 		"");
				break;
			}
			case UnOpNode::unMinus: {
				addTcRule("",		"integer",	"integer");
				addTcRule(M_ELSE,	"",			"double");
				/*CARD*/		
				addCdRule(META,		"x..y",		"1..1");
				/*T-NAME*/		
				addTnRule(META,		"X",		"");
				break;
			}
			case UnOpNode::boolNot: { 
				addTcRule(META,		"any",		"boolean");
				/*CARD*/		
				addCdRule(META,		"x..y",		"1..1");
				/*T-NAME*/		
				addTnRule(META,		"X",		"");
				break;
			}
			case UnOpNode::deleteOp: { 
				addTcRule("",		"ref",		"void"); 	//only refs may be removed.
				addTcRule(M_ELSE,	"",			TC_RS_ERROR);
				addCdRule(META,	"",			"1..1");	
				break;
			}
			default: break;
			}
			break;
		case TreeNode::TNAS: 
				addTcRule(META, 	"Base",		BS_NAMEAS);
				addCdRule(META,		"CD",		CD_NAMEAS);
				addTnRule(META,		"TN",		"");
				addCkRule(META,		"CK",		CK_NAMEAS);	
				break;
		case TreeNode::TNCREATE: {
				addTcRule("",		"binder",	BS_CREATE);	//returns sigref - to the created object.
				addTcRule("",		"ref",		BS_COPY);	// decent sig to return as restore result
				addTcRule(M_ELSE,	"",			TC_RS_ERROR);
				addCdRule(META,		"CD",		ARG_COPY);	
				addCkRule(META,		"CK",		ARG_COPY);
				break;
		}
		default: break;
		}
		provideReturnLinks();
	}
	
	/* **************** end of Restore dec. table rules 	********************** */
	
}


