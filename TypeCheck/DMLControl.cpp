
#include "DMLControl.h"
#include "../QueryParser/Deb.h"
#include "TypeChecker.h"


namespace TypeCheck {

	int DMLControl::init() {
		vector<ObjectPointer *> * roots;
		vector<ObjectPointer *> * rootTypes;
		bool newTransaction = false;
		if (tr == NULL) {
			Deb::ug("Have to create new transaction to init control structures!");
			TransactionManager::getHandle()->createTransaction(tr);
			newTransaction = true;
		}
		tr->getRoots(TC_MDN_NAME, roots);	
		tr->getRoots(TC_MDNT_NAME, rootTypes);
		if (Deb::ugOn()) cout <<"got all the roots / root types"<< endl;
		
		if (Deb::ugOn()) cout << "controL: " << roots->size() << " obiektow root MDN"<< endl;
		if (Deb::ugOn()) cout << "controL DEFTYPES: " << rootTypes->size() << " obiektow rootType MDNT"<< endl;
		registerRootMdns(roots, DMLControl::object);
		registerRootMdns(rootTypes, DMLControl::type);
		Deb::ug("deleting root/rootType vectors on finishing dmlStructs init(). ");
		delete roots;
		delete rootTypes;
		if (newTransaction) tr->commit();
	}
	
	int DMLControl::registerRootMdns(vector<ObjectPointer *> *vec, MdnRootKind mdnKind) {
		for (unsigned int i = 0; i < vec->size(); i++){
			ObjectPointer * mdnOp = (*vec)[i];
			string rootName = "";
			vector<LogicalID*> *v = mdnOp->getValue()->getVector();
			registerRootMdnsRec(v, rootName, mdnKind, true);
		}
	}
			
	int DMLControl::registerRootMdnsRec(vector<LogicalID*> *v, string rootName, MdnRootKind mdnKind, bool topLevel) {
		if (topLevel) {
			for (unsigned int j = 0; j < v->size(); j++) {
				ObjectPointer *op = NULL;
				tr->getObjectPointer( (*v)[j], Store::Read, op,false);
				if (op->getName() == "name" && topLevel) {
					rootMdns[op->getValue()->getString()] = mdnKind;
					rootName = op->getValue()->getString();
					break;
				}
			}
		}
		if (mdnKind == DMLControl::type) {
			for (unsigned int j = 0; j < v->size(); j++) {
				ObjectPointer *op = NULL;
				tr->getObjectPointer( (*v)[j], Store::Read, op,false);
				if (op->getName() == "typeName") {
					string tName = op->getValue()->getString();
					markDependency(rootName, tName);
				} else if (op->getName() == "subobject") {
					ObjectPointer * pom;
					tr->getObjectPointer(op->getLogicalID(), Store::Read, pom, false);
					vector<LogicalID*>* vSubs = pom->getValue()->getVector();
					registerRootMdnsRec(vSubs, rootName, mdnKind, false);
				}
			}
		}
	}
		
	int DMLControl::findRoot(string name) {
		if (rootMdns.find(name) == rootMdns.end()) return DMLControl::absent;
		return rootMdns[name];
	}
	
	void DMLControl::addRoot(string name, MdnRootKind mdnKind) {
		rootMdns[name] = mdnKind;
	}
	
	DMLControl::~DMLControl() {
		for (map<string, vector<string>* >::iterator iter = typeDeps.begin(); iter != typeDeps.end(); iter++) {
			delete (iter->second);
		}
	}
	
	void DMLControl::markDependency(string masterType, string subType) {
		if (typeDeps.find(masterType) == typeDeps.end()) {
			typeDeps[masterType] = new vector<string>();
		}
		vector<string> *v = typeDeps[masterType];
		for (unsigned int i = 0; i < v->size(); i++) {
			if ((*v)[i] == subType) return;
		}
		v->push_back(subType);
	}
	//return 0 if masterType does not depend on subType, else - sth. other than 0 
	int DMLControl::findDependency (string masterType, string subType) {
		// no self-recurrence
		if (masterType == subType) return 1;
		// have to BFS through the map of typeDeps...
		if (typeDeps.find(masterType) == typeDeps.end()) return 0;
		vector<string> *queue = new vector<string>();
		vector<string> *vec = typeDeps[masterType];
		queue->insert(queue->end(), vec->begin(), vec->end());
		//assuming no cycles may appear here. (thats why we're having these structures).
		while (queue->size() > 0) {
			cout << "in while loop, queue size: " << queue->size() << endl;
			string name = queue->at(0);
			queue->erase(queue->begin());
			if (name == subType) return 1;
			if (typeDeps.find(name) != typeDeps.end()) {
				vector<string> *v = typeDeps[name];
				queue->insert(queue->end(), v->begin(), v->end() );
			}
		}
		return 0;
	}
	
	DMLControl::DMLControl(Transaction *t) {tr = t;}
	
	void DMLControl::setTransaction(Transaction *t) {tr = t;};
	
	string DMLControl::depsToString() {
		string ret = "TYPE DEPENDENCIES: typeA --> tB, tC :: typeA DEPENDS_ON tB & tC.\n";
		map<string, vector<string> * >::iterator iter;
		for (iter = typeDeps.begin(); iter != typeDeps.end(); iter++) {
			ret += "*" + iter->first + " --> ";
			vector<string> *v = iter->second;
			for (unsigned int i = 0; i < v->size(); i++) {
				if (i > 0) ret += ", ";
				ret += v->at(i);
			}
			ret += "\n";
		}
		return ret;
	}
	string DMLControl::rootMdnsToString() {
		string ret = "EXISTING MDN ELEMENTS : \n";
		for (map<string, MdnRootKind>::iterator iter = rootMdns.begin(); iter != rootMdns.end(); iter++) {
			ret += "[" + iter->first + ",";
			if (iter->second == DMLControl::object) ret += "object]  ";
			else if (iter->second == DMLControl::type) ret += "typedef]  ";
			else ret += " _?_ ]  ";
		}
		ret += "\n";
		return ret;
	}

}

