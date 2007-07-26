#include "ClassGraph.h"

using namespace QParser;
using namespace SessionDataNms;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;


namespace QExecutor
{

ClassGraph* ClassGraph::handle = NULL;

ErrorConsole *ClassGraph::ec = NULL;

void ClassGraph::shutdown() {
	if(handle != NULL) {
		delete handle;
	}
}

int ClassGraph::getHandle(ClassGraph*& cg) {
	cg = handle;
	return 0;
}

int ClassGraph::init() {
	ec = new ErrorConsole("QueryExecutor");
	int errcode = 0;
	Transaction *tr;
	errcode = (TransactionManager::getHandle())->createTransaction(tr);
	if (errcode != 0) {
		ec->printf("Error in loading Class Graph.");
		return errcode;
	}
	vector<LogicalID*>* classesLids;
	errcode = tr->getClassesLID(classesLids);
	if (errcode != 0) {
		ec->printf("Error in loading Class Graph.");
		return errcode;
	}
	ClassGraph* tmp = new ClassGraph();
	for(vector<LogicalID*>::iterator i = classesLids->begin(); i != classesLids->end(); ++i) {
		errcode = tmp->completeSubgraph(*i, tr, NULL);
		if (errcode != 0) return errcode;
	}
	ClassGraph::handle = tmp;
	return 0;
}

string ClassGraphVertex::classSetToString(ClassGraph* cg, SetOfLids* classSet) {
	ostringstream str;
	for(SetOfLids::iterator i = classSet->begin(); i != classSet->end(); i++) {
		if(cg->vertexExist(*i)) {
			ClassGraphVertex* cgv;
			cg->getVertex(*i, cgv);
			str << " " << cgv->name;
		} else {
			str << " 'class not exist'";
		}
	}
	return str.str();
}

string ClassGraphVertex::toString(ClassGraph* cg){
 	ostringstream str;
	str << "Name: " << name << ", instance: " << invariant << ", extends: [";
	str << classSetToString(cg, &extends);
	str << " ], subclasses: [";
	str << classSetToString(cg, &subclasses);
	str << " ]";
	return str.str();
}

string ClassGraph::toString() {
	ostringstream str;
	str << "classes:" << endl;
	for(MapOfClassVertices::iterator i = classGraph.begin(); i != classGraph.end(); i++) {
		ClassGraphVertex* cgv = (*i).second;
		str << " " << cgv->toString(this) << endl;
	}
	str << "invariants:" << endl;
	for(MapOfInvariantVertices::iterator i = invariants.begin(); i != invariants.end(); i++) {
		str << " " << (*i).first << " classes: [";
		str << ClassGraphVertex::classSetToString(this, (*i).second);
		str << " ] " << endl;
	}
	return str.str();
}

int ClassGraphVertex::initNotGraphProperties(ObjectPointer *optr, Transaction *&tr, QueryExecutor *qe) {
	int errcode = 0;
	DataValue* dv = optr->getValue();
	name = optr->getName();
	if(dv->getSubtype() != Store::Class) {
		return 1;//spodziewana klasa, otrzymano cos innego
	}
	vector<LogicalID*>* inner_vec = dv->getVector();
	int amountOfNeededValues = 1;
	for (unsigned int i=0; i < inner_vec->size(); i++) {
		if(amountOfNeededValues <= 0) break; 
		ObjectPointer *inner_optr;
		errcode = tr->getObjectPointer (inner_vec->at(i), Store::Read, inner_optr, false);
		if (errcode != 0) {
			return ClassGraph::trErrorOccur(qe, "[QE] Error in getObjectPointer.", errcode);
		}
		if (inner_optr->getName() == QE_INVARIANT_BIND_NAME) {
			invariant = (inner_optr->getValue())->getString();
			amountOfNeededValues--;
		}
		/*if (inner_optr->getName() == QE_CLASS_BIND_NAME) {
			name = (inner_optr->getValue())->getString();
			amountOfNeededValues--;
		}*/
	}
	return 0;
}

int ClassGraph::invariantToCohesiveSubgraph(string& invariantName, Transaction *&tr, QueryExecutor *qe) {
	vector<LogicalID*>* lids;
	int errcode = tr->getClassesLIDByInvariant(invariantName, lids);
	if(errcode != 0) {
		return trErrorOccur(qe,  "[QE] Error in geting class by invariant.", errcode );
	}
	for(vector<LogicalID*>::iterator i = lids->begin(); i != lids->end(); i++) {
		(*ec) << "^^^^^^^^^^^^^^^ ------------ " << (*i)->toString();
		//zakladamy, ze jak klasa jest to jest z calym podgrafem
		//wiec trzeba cos robic tylko gdy klasy nie ma
		if(classGraph.find(*i) == classGraph.end()) {
			errcode = completeSubgraph((*i), tr, qe);
			if(errcode != 0) {
				return trErrorOccur(qe,  "[QE] Error in geting class.", errcode );
			}
		}
	}
	return 0;
}

void ClassGraph::putToInvariants(string& invariantName, LogicalID* lid) {
	if(invariantName.empty()) {
		return;
	}
	SetOfLids* slids = NULL;
	MapOfInvariantVertices::iterator slidsPtr = invariants.find(invariantName);
	if(slidsPtr != invariants.end()) {
		slids = (*slidsPtr).second;
	}
	if(slids == NULL) {
		slids = new SetOfLids();
		invariants[invariantName] = slids;
	}
	slids->insert(lid);
}

void ClassGraph::removeFromInvariant(LogicalID* lid, string name) {
	MapOfInvariantVertices::iterator invI = invariants.find(name);
	if(invI == invariants.end()) {
		return;
	}
	(*invI).second->erase(lid);
	if((*invI).second->empty()) {
		delete (*invI).second;
	}
	invariants.erase(invI);
}

void ClassGraph::removeFromSubclasses(SetOfLids* extendedClasses, LogicalID* lid) {
	for(SetOfLids::iterator i = extendedClasses->begin(); i != extendedClasses->end(); ++i) {
		MapOfClassVertices::iterator cgvI = classGraph.find(lid);
		if(cgvI != classGraph.end()) {
			(*cgvI).second->removeSubclass(lid);
		}
	}
}

void ClassGraph::removeFromExtends(SetOfLids* subclasses, LogicalID* lid) {
	for(SetOfLids::iterator i = subclasses->begin(); i != subclasses->end(); ++i) {
		MapOfClassVertices::iterator cgvI = classGraph.find(lid);
		if(cgvI != classGraph.end()) {
			(*cgvI).second->removeExtend(lid);
			(*cgvI).second->invalid = true;
		}
	}
}

int ClassGraph::removeClass(LogicalID* lid) {
	MapOfClassVertices::iterator cgvI = classGraph.find(lid);
	if(cgvI == classGraph.end()) {
		return 0;
	}
	
	removeFromInvariant(lid, (*cgvI).second->invariant);
	removeFromSubclasses(&((*cgvI).second->extends), lid);
	removeFromExtends(&((*cgvI).second->subclasses), lid);
	
	LogicalID* lidToDel = (*cgvI).first;
	
	delete (*cgvI).second;
	classGraph.erase(cgvI);
	delete lidToDel;
	return 0;
}

int ClassGraph::addClass(ObjectPointer *optr, Transaction *&tr, QueryExecutor *qe) {
	LogicalID* newLid = optr->getLogicalID()->clone();
	DataValue* dv = optr->getValue();
	ClassGraphVertex* cgv = new ClassGraphVertex();
	cgv->initNotGraphProperties(optr, tr, qe);
	classGraph[newLid] = cgv;
	putToInvariants(cgv->invariant, newLid);
	SetOfLids* tmp = dv->getClassMarks();
	for(SetOfLids::iterator i = tmp->begin(); i != tmp->end(); ++i) {
		MapOfClassVertices::iterator extCgvI = classGraph.find(*i);
		if(extCgvI == classGraph.end()) {
			removeClass(newLid);
			return qe->qeErrorOccur("[ClassGraph] No class in graph.", ENoClassDefFound);
		}
		((*extCgvI).second)->addSubclass(newLid);
		cgv->addExtend((*extCgvI).first);
	}
	return 0;
}


int ClassGraph::completeSubgraph(LogicalID* lid, Transaction *&tr, QueryExecutor *qe) {
	if(classGraph.find(lid) != classGraph.end()) {
		return 0;
	}
	LogicalID* newLid = lid->clone();
	ObjectPointer *optr;
	int errcode = tr->getObjectPointer(newLid, Store::Read, optr, false); 
	if (errcode != 0) {
			delete newLid;
			return trErrorOccur(qe, "[QE] Error in geting class (from lid).", errcode );
	}
	DataValue* dv = optr->getValue();
	ClassGraphVertex* cgv = new ClassGraphVertex();
	cgv->initNotGraphProperties(optr, tr, qe);
	classGraph[newLid] = cgv;
	putToInvariants(cgv->invariant, newLid);
	SetOfLids* tmp = dv->getClassMarks();
	for(SetOfLids::iterator i = tmp->begin(); i != tmp->end(); ++i) {
		MapOfClassVertices::iterator subclassPtr = classGraph.find(*i);
		if(subclassPtr == classGraph.end()) {
			errcode = completeSubgraph(*i, tr, qe);
			if(errcode != 0) return errcode;
			subclassPtr = classGraph.find(*i);
		}
		cgv->addExtend( (*subclassPtr).first );
	}
	tmp = dv->getSubclasses();
	for(SetOfLids::iterator i = tmp->begin(); i != tmp->end(); ++i) {
		MapOfClassVertices::iterator subclassPtr = classGraph.find(*i);
		if(subclassPtr == classGraph.end()) {
			errcode = completeSubgraph(*i, tr, qe);
			if(errcode != 0) return errcode;
			subclassPtr = classGraph.find(*i);
		}
		cgv->addSubclass( (*subclassPtr).first );
	}

	return 0;
}

int ClassGraph::fetchSubInvariantNames(string& invariantName, Transaction *&tr, QueryExecutor *qe, stringHashSet& invariantsNames,  bool noInvariantName) {
	return fetchInvariantNames(invariantName, tr, qe, invariantsNames, noInvariantName, true);
}

int ClassGraph::fetchExtInvariantNames(string& invariantName, Transaction *&tr, QueryExecutor *qe, stringHashSet& invariantsNames,  bool noInvariantName) {
	return fetchInvariantNames(invariantName, tr, qe, invariantsNames, noInvariantName, false);
}

int ClassGraph::fetchExtInvariantNamesForLid(LogicalID* lid, Transaction *&tr, QueryExecutor *qe, stringHashSet& invariantsNames,  bool noObjectName) {
	ObjectPointer *optr;
	int errcode = tr->getObjectPointer(lid, Store::Read, optr, false); 
	if (errcode != 0) {
			return trErrorOccur(qe, "[QE] Error in geting object.", errcode );
	}
	string objectName = optr->getName();
	SetOfLids* objectClasses = optr->getValue()->getClassMarks();
	invariantsNames.clear();
	if(objectClasses == NULL) {
		return 0;
	}
	for(SetOfLids::iterator i = objectClasses->begin(); i != objectClasses->end(); ++i) {
		MapOfClassVertices::iterator classI = classGraph.find(*i);
		if(classI != classGraph.end()) {
			ClassGraphVertex* cgv = (*classI).second;
			if(!cgv->invariant.empty() && cgv->invariant == objectName) {
				errcode = fetchInvariantNames(cgv, invariantsNames, false);
				if(errcode != 0) return errcode;
			}
		}
	}
	if(noObjectName) {
		invariantsNames.erase(objectName);
	}
	return 0;
}

int ClassGraph::fetchInvariantNames(string& invariantName, Transaction *&tr, QueryExecutor *qe, stringHashSet& invariantsNames, bool noInvariantName, bool sub) {
	string dirCaption = (sub)?("sub"):("ext");
	ec->printf("[QE] fetching %sInvariants of invariant: %s", dirCaption.c_str(), invariantName.c_str());
	int errcode = 0;
	if(lazy) {
		errcode = invariantToCohesiveSubgraph(invariantName, tr, qe);
		if(errcode != 0) return errcode;
	}
	errcode = fetchInvariantNames(invariantName, invariantsNames, sub);
	if(errcode != 0) return errcode;
	if(noInvariantName) {
		invariantsNames.erase(invariantName);
	}
	ec->printf("[QE] fetching %sInvariants: %i", dirCaption.c_str(), invariantsNames.size());
	/*for(stringHashSet::iterator i = invariantsNames.begin(); i != invariantsNames.end(); i++)
		(*ec)<<(*i);*/
	return 0;
}

int ClassGraph::fetchInvariantNames(string& invariantName, stringHashSet& invariantsNames, bool sub) {
	MapOfInvariantVertices::iterator slidsPtr = invariants.find(invariantName);
	if(slidsPtr == invariants.end()) {
		return 0;
	}
	SetOfLids* slids = (*slidsPtr).second;
	for(SetOfLids::iterator i = slids->begin(); i != slids->end(); i++) {
		ClassGraphVertex* cgv;
		int errcode = getVertex(*i, cgv);
		if(errcode != 0) return errcode;
		errcode = fetchInvariantNames(cgv, invariantsNames, sub);
		if(errcode != 0) return errcode;
	}
	return 0;
}

int ClassGraph::fetchInvariantNames(ClassGraphVertex* cgvIn, stringHashSet& invariantsNames, bool sub) {
	SetOfLids& lids = (sub)?(cgvIn->subclasses):(cgvIn->extends);
	for(SetOfLids::iterator i = lids.begin(); i != lids.end(); ++i) {
		ClassGraphVertex* cgv;
		int errcode = getVertex(*i, cgv);
		if(errcode != 0) {
			return errcode;
		}
		if(!(cgv->invariant).empty()) {
			invariantsNames.insert(cgv->invariant);
		}
		errcode = fetchInvariantNames(cgv, invariantsNames, sub);
		if(errcode != 0) return errcode;
	}
	return 0;
}

/*int ClassGraph::classBelongsToInvariant(LogicalID* lid, string& invariantUpName, bool& inInvariant) {
	
}*/

int ClassGraph::belongsToInvariant(SetOfLids* extClasses, string& invariantUpName, bool& inInvariant, bool isObject) {
	int errcode;
	if(extClasses == NULL || invariantUpName.empty()) {
		inInvariant = false;
		return 0;
	}
	for(SetOfLids::iterator i = extClasses->begin(); i != extClasses->end(); ++i) {
		if(!vertexExist(*i)) continue;
		ClassGraphVertex* cgv;
		errcode = getVertex(*i, cgv);
		if(errcode != 0) return errcode;
		// Jesli obiekt jest instancja klasy ktora nie ma invariantu to nie podlega podmienianiu na poziomie kolekcji.
		// Trzeba sprawdzic inne klasy obiektu, a dla klas trzeba ten warunek pominac.
		if(isObject && (cgv->invariant).empty()) continue;
		if(cgv->invariant == invariantUpName) {
			inInvariant = true;
			return 0;
		}
		if(!(cgv->extends).empty()) {
			errcode = belongsToInvariant(&(cgv->extends), invariantUpName, inInvariant, false);
			if(errcode != 0) return errcode;
			if(inInvariant) return 0;
		}
	}
	inInvariant = false;
	return 0;
}
		
int ClassGraph::belongsToInvariant(LogicalID* lid, string& invariantUpName, Transaction *&tr, QueryExecutor *qe, bool& inUpInvariant) {
	if(invariantUpName.empty()) {
		inUpInvariant = false;
		return 0;
	}
	MapOfInvariantVertices::iterator slidsPtr = invariants.find(invariantUpName);
	if(slidsPtr == invariants.end()) {
		inUpInvariant = false;
		return 0;
	}
	ObjectPointer *optr;
	int errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
	if (errcode != 0) {
		return trErrorOccur(qe, "[QE] Error in getObjectPointer.", errcode);
	}
	return belongsToInvariant((optr->getValue())->getClassMarks(), invariantUpName, inUpInvariant);
}

int ClassGraph::trErrorOccur(QueryExecutor* qe, string msg, int errcode ) {
	if(qe != NULL) return qe->trErrorOccur(msg, errcode);
	*ec << msg;
	return errcode;
}

ClassGraph::~ClassGraph() {
	vector<LogicalID*> lidsToDel;
	for(MapOfClassVertices::iterator i = classGraph.begin(); i != classGraph.end(); i++) {
		lidsToDel.push_back((*i).first);
		delete (*i).second;
	}
	for(MapOfInvariantVertices::iterator i = invariants.begin(); i != invariants.end(); i++) {
		//Nie nalezy niszczyc tego co jest w SetOfLids,
		//gdyz to sa wskazniki do obiektow z classGraph.
		delete (*i).second;
	}
	delete ec;
	for(vector<LogicalID*>::iterator i = lidsToDel.begin(); i != lidsToDel.end(); ++i) {
		delete (*i);
	}
}

}
