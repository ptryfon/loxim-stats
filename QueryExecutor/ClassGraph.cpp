#include "ClassGraph.h"

using namespace QParser;
using namespace SessionDataNms;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;


namespace QExecutor
{

int ClassGraphVertex::initNotGraphProperties(DataValue* dv, Transaction *&tr, QueryExecutor *qe) {
	int errcode = 0;
	if(dv->getSubtype() != Store::Class) {
		return 1;//spodziewana klasa, otrzymano cos innego
	}
	vector<LogicalID*>* inner_vec = dv->getVector();
	int amountOfNeededValues = 2;
	for (unsigned int i=0; i < inner_vec->size(); i++) {
		if(amountOfNeededValues <= 0) break; 
		ObjectPointer *inner_optr;
		errcode = tr->getObjectPointer (inner_vec->at(i), Store::Read, inner_optr, false);
		if (errcode != 0) {
			return qe->trErrorOccur("[QE] Error in getObjectPointer.", errcode);
		}
		if (inner_optr->getName() == QE_INVARIANT_BIND_NAME) {
			invariant = (inner_optr->getValue())->getString();
			amountOfNeededValues--;
		}
		if (inner_optr->getName() == QE_CLASS_BIND_NAME) {
			name = (inner_optr->getValue())->getString();
			amountOfNeededValues--;
		}
	}
	return 0;
}

int ClassGraph::invariantToCohesiveSubgraph(string& invariantName, Transaction *&tr, QueryExecutor *qe) {
	vector<LogicalID*>* lids;
	int errcode = tr->getClassesLIDByInvariant(invariantName, lids);
	if(errcode != 0) {
		return qe->trErrorOccur( "[QE] Error in geting class by invariant.", errcode );
	}
	for(vector<LogicalID*>::iterator i = lids->begin(); i != lids->end(); i++) {
		(*ec) << "^^^^^^^^^^^^^^^ ------------ " << (*i)->toString();
		//zakladamy, ze jak klasa jest to jest z calym podgrafem
		//wiec trzeba cos robic tylko gdy klasy nie ma
		if(classGraph.find(*i) == classGraph.end()) {
			errcode = completeSubgraph((*i), tr, qe);
			if(errcode != 0) {
				return qe->trErrorOccur( "[QE] Error in geting class.", errcode );
			}
		}
	}
	return 0;
}

void ClassGraph::putToInvariants(string& invariantName, LogicalID* lid) {
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


int ClassGraph::completeSubgraph(LogicalID* lid, Transaction *&tr, QueryExecutor *qe) {
	if(classGraph.find(lid) != classGraph.end()) {
		return 0;
	}
	LogicalID* newLid = lid->clone();
	ObjectPointer *optr;
	int errcode = tr->getObjectPointer(newLid, Store::Read, optr, false); 
	if (errcode != 0) {
			return qe->trErrorOccur( "[QE] Error in geting class (from lid).", errcode );;
	};
	DataValue* dv = optr->getValue();
	ClassGraphVertex* cgv = new ClassGraphVertex();
	cgv->initNotGraphProperties(dv, tr, qe);
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

int ClassGraph::fetchSubInvariantNames(string& invariantName, Transaction *&tr, QueryExecutor *qe, stringHashSet& invariantsNames, bool noInvariantName) {
	(*ec)<<"[QE] fetching subInvariants of invariant: " << invariantName;
	int errcode = invariantToCohesiveSubgraph(invariantName, tr, qe);
	if(errcode != 0) return errcode;
	errcode = fetchSubInvariantNames(invariantName, invariantsNames);
	if(errcode != 0) return errcode;
	if(noInvariantName) {
		invariantsNames.erase(invariantName);
	}
	ec->printf("[QE] fetching subInvariants: %i", invariantsNames.size());
	for(stringHashSet::iterator i = invariantsNames.begin(); i != invariantsNames.end(); i++)
		(*ec)<<(*i);
	return 0;
}

int ClassGraph::fetchSubInvariantNames(string& invariantName, stringHashSet& invariantsNames) {
	MapOfInvariantVertices::iterator slidsPtr = invariants.find(invariantName);
	if(slidsPtr == invariants.end()) {
		return 0;
	}
	SetOfLids* slids = (*slidsPtr).second;
	for(SetOfLids::iterator i = slids->begin(); i != slids->end(); i++) {
		ClassGraphVertex* cgv;
		int errcode = getVertex(*i, cgv);
		if(errcode != 0) return errcode;
		errcode = fetchSubInvariantNames(cgv, invariantsNames);
		if(errcode != 0) return errcode;
	}
	return 0;
}

int ClassGraph::fetchSubInvariantNames(ClassGraphVertex* cgvIn, stringHashSet& invariantsNames) {
	for(SetOfLids::iterator i = (cgvIn->subclasses).begin(); i != (cgvIn->subclasses).end(); ++i) {
		ClassGraphVertex* cgv;
		int errcode = getVertex(*i, cgv);
		if(errcode != 0) {
			return errcode;
		}
		if(!(cgv->invariant).empty()) {
			invariantsNames.insert(cgv->invariant);
		}
		errcode = fetchSubInvariantNames(cgv, invariantsNames);
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
		return qe->trErrorOccur("[QE] Error in getObjectPointer.", errcode);
	}
	return belongsToInvariant((optr->getValue())->getClassMarks(), invariantUpName, inUpInvariant);
}

ClassGraph::~ClassGraph() {
	vector<LogicalID*> lidsToDel;
	for(MapOfClassVertices::iterator i = classGraph.begin(); i != classGraph.end(); i++) {
		lidsToDel.push_back((*i).first);
		delete (*i).second;
	}
	for(MapOfInvariantVertices::iterator i = invariants.begin(); i != invariants.end(); i++) {
		//Nie nalezy niszczyc tego co jest w SetOfLids,
		//gdyz to sa wskazniki do obiektow zniszczonych juz wyzej.
		delete (*i).second;
	}
	delete ec;
	for(vector<LogicalID*>::iterator i = lidsToDel.begin(); i != lidsToDel.end(); ++i) {
		delete (*i);
	}
}

}
