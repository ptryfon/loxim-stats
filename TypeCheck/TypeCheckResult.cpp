#include "TypeCheckResult.h"
#include "QueryParser/Stack.h"

#include <vector>
#include <sstream>
using namespace std;

namespace TypeCheck 
{

	TypeCheckResult::TypeCheckResult(string res) {
		setResultSig(res);
		//All other signatures will require a resultGenerator to form a new base signature.
	}		
	
	string TypeCheckResult::setEffect(string e) {
		if ((this->effect != "ERROR") && (e != "SUCCESS")) this->effect = e;
		return this->effect;
	}

	void TypeCheckResult::setResultSig(string res) {
		if (res == "integer") resultSig = new SigAtomType("int");
		if (res == "double") resultSig = new SigAtomType("double");
		if (res == "boolean") resultSig = new SigAtomType("boolean");
		if (res == "string") resultSig = new SigAtomType("string");
		//the case below will not be valid any more.
		if (res == "bag" || res == "list") {
			resultSig = new SigColl();
			((SigColl *)resultSig)->setCollTypeByString(res);
		}
	}
	
	void TypeCheckResult::addActionId(int actId) {
		if (actId >= 0)	this->actionIds.push_back(actId);
	}
	
	string TypeCheckResult::printAllInfo() {
		string str = "";
		str += "[sig: " +(getSig() != NULL ? getSig()->toString() : "NULL") + "] \n ";
		str += "[effect: " + getEffect() + "] \n ";
		stringstream sout;
		string dynStr = (dynCtrl == true ? "true" : "false");
		str += "[dynCtrl: " + dynStr + "] \n ";
		
		sout << actionIds.size();
		str += "[actions: " + sout.str() + "] \n ";
		sout.str("");
		for (unsigned int i = 0; i < actionIds.size(); i++) {
			if (i > 0) str += ", ";
			sout << actionIds.at(i);
			str += sout.str();
			sout.str("");
		} str += "\n";
		return str;
	}
	
	void TypeCheckResult::clear() {
		resultSig = NULL; effect = ""; dynCtrl = false;
	}
	
	TypeCheckResult::~TypeCheckResult() {
		if (resultSig != NULL) { 
			delete resultSig;
			cout << "deleted resultSig in tcResult" << endl;
		}
	}


}

