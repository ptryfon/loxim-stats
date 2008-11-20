#include <TypeCheck/TypeCheckResult.h>
#include <QueryParser/Stack.h>
#include <TypeCheck/DecisionTable.h>

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
		if (res == "void")	resultSig = new SigVoid();
		//the case below will not be valid any more.
		if (res == "bag" || res == "list") {
			resultSig = new SigColl();
			((SigColl *)resultSig)->setCollTypeByString(res);
		}
	}
	
	void TypeCheckResult::addActionId(ActionStruct action) {
		if (action.id >= 0) this->actionIds.push_back(action);
	}
	
	void TypeCheckResult::addActionId(int actId, int actArg) {
		if (actId >= 0)	{
			ActionStruct action = {actId, actArg};
			this->actionIds.push_back(action);
		}
	}
	
	void TypeCheckResult::addErrorPart(string part) {
		this->errorParts.push_back(part);
	}
	
	string TypeCheckResult::printAllInfo() {

		string str = "";
		str += "[sig: " +(getSig() != NULL ? getSig()->toString() : "NULL") + "] \n ";
		str += "[effect: " + getEffect() + "] \n ";
		stringstream sout;
		string dynStr = (dynCtrl == true ? "true" : "false");
		str += "[dynCtrl: " + dynStr + "] \n ";
		
		sout << actionIds.size();
		str += "[actions: " + sout.str() + "] ";
		sout.str("");
		
		for (unsigned int i = 0; i < actionIds.size(); i++) {
			if (i > 0) str += ", ";
			sout << actionIds.at(i).id;
			str += sout.str();
			sout.str("");
		} str += "\n";
		if (coerceSig != NULL) {
			str += "[coerceSig: " + coerceSig->toString() + "]\n";
		}
		if (errorParts.size() > 0) {
			str += "[errorPts: ";
			for (unsigned int i = 0; i < errorParts.size(); i++) {
				if (i > 0) str += ", ";
				str += errorParts.at(i);
			} str += "]\n";
		} else {str += "No errorParts\n";}
		return str;
	}
	
	void TypeCheckResult::clear() {
		if (resultSig != NULL) delete resultSig;
		if (coerceSig != NULL) delete coerceSig;
		resultSig = NULL; coerceSig = NULL; effect = ""; dynCtrl = false; errorParts.clear(); actionIds.clear();
	}
	void TypeCheckResult::fill(TypeCheckResult &tcr) {
		tcr.clear();
		if (resultSig != NULL) tcr.setSig(resultSig->clone());
		if (coerceSig != NULL) tcr.setCoerceSig(coerceSig->clone());
		for (unsigned int i = 0; i < actionIds.size(); i++) tcr.addActionId(actionIds.at(i));
		for (unsigned int i = 0; i < errorParts.size(); i++) tcr.addErrorPart(errorParts.at(i));
		tcr.setEffect(effect);
		tcr.setDynCtrl(dynCtrl);
	}
	
	bool TypeCheckResult::isBaseError() {
		return (this->isError() && errorParts.size() > 0 && errorParts.at(0) == SIG_BASE);
	}
	
	TypeCheckResult::~TypeCheckResult() {
		if (resultSig != NULL) delete resultSig;
		if (coerceSig != NULL) delete coerceSig;
	}


}

