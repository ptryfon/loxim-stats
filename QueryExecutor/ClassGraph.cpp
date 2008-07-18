#include "ClassGraph.h"
#include "BindNames.h"

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;


namespace QExecutor
{
	
string ClassGraph::classNameToExtPrefix(const string& className) { return className + QE_NAMES_SEPARATOR;}

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
	//transakcja nie byla konczona
	if ((errcode = tr->commit())) {
		return errcode;
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

string ClassGraphVertex::fieldsToString(stringHashSet& fields) {
	ostringstream str;
	for(stringHashSet::iterator i = fields.begin(); i != fields.end(); ++i) {
		str << " " << (*i);
	}
	return str.str();
}

string ClassGraphVertex::methodsToString() {
	ostringstream str;
	for(NameToArgCountToMethodMap::iterator i = methods.begin(); i != methods.end(); ++i) {
		str <<" (";
		ArgsCountToMethodMap* map = (*i).second;
		for(ArgsCountToMethodMap::iterator j = map->begin(); j != map->end(); ++j) {
			str << " " << (*j).first;
		}
		str << " )" << (*i).first;
	}
	return str.str();
}

string ClassGraphVertex::toString(ClassGraph* cg){
 	ostringstream str;
	str << "Name: " << name << ", instance: " << invariant << ", extends: [";
	str << classSetToString(cg, &extends);
	str << " ], subclasses: [";
	str << classSetToString(cg, &subclasses);
	str << " ], fields: [";
	str << fieldsToString(fields);
	str << " ], static fields: [";
	str << fieldsToString(staticFields);
	str << " ], methods: [";
	str << methodsToString();
	str << "]";
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

bool ClassGraphVertex::hasStaticField(string& staticField) {
	stringHashSet::iterator i = staticFields.find(staticField);
	return i != staticFields.end();
}

int ClassGraphVertex::fetchMethod(LogicalID* lid, Transaction *&tr, QueryExecutor *qe, Method*& method, unsigned int & params_count, string& name) {
	int errcode;
	ObjectPointer *optr;
	
	errcode = tr->getObjectPointer (lid, Store::Read, optr, false); 
	if (errcode != 0) {
		return ClassGraph::trErrorOccur(qe, "[ClassGraph] initMethod - Error in getObjectPointer.", errcode);
	}
	
	DataValue* data_value = optr->getValue();
	name = optr->getName();
	int vType = data_value->getType();
	ExtendedType extType = data_value->getSubtype();
	
	int procBody_count = 0;

	params_count = 0;
	int others_count = 0;

	if ((vType == Store::Vector) && (extType == Store::Procedure)) {
		method = new Method(lid->clone());
		vector<LogicalID*>* tmp_vec = (data_value->getVector());
		int vec_size = tmp_vec->size();
		for (int i = 0; i < vec_size; i++ ) {
			LogicalID *tmp_logID = tmp_vec->at(i);
			ObjectPointer *tmp_optr;
			if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr, false)) != 0) {
				return ClassGraph::trErrorOccur(qe, "[ClassGraph] initMethod - Error in getObjectPointer.", errcode);
			}
			string tmp_name = tmp_optr->getName();
			DataValue* tmp_data_value = tmp_optr->getValue();
			int tmp_vType = tmp_data_value->getType();
			if (tmp_vType == Store::String) {
				if (tmp_name == "ProcBody") {
					method->code = tmp_data_value->getString();
					procBody_count++;
				}
				else if (tmp_name == "Param") {
					method->params.push_back(tmp_data_value->getString());
					params_count++;
				}
				else others_count++;
			}
		}
	}
	
	return 0;
}

int ClassGraphVertex::insertIntoMethods(string& name, unsigned int argsCount, Method* m) {
	NameToArgCountToMethodMap::iterator methodI = methods.find(name);
	ArgsCountToMethodMap* map; 
	if(methodI == methods.end()) {
		map = new ArgsCountToMethodMap();
		methods[name] = map;
		(*map)[argsCount] = m;
		return 0;
	} 
	map = (*methodI).second;
	if(map->find(argsCount) != map->end()) {
		//TODO: blad zduplikowane metody
		delete m;// nie wkladamy wiec tracimy wskaznik
		return 1;
	}
	(*map)[argsCount] = m;
	return 0;
}

int ClassGraphVertex::putMethod(LogicalID* lid, Transaction *&tr, QueryExecutor *qe) {
	Method* method;
	unsigned int params_count = 0;
	string name;
	int errcode = fetchMethod(lid, tr, qe, method, params_count, name);
	if(errcode != 0) return errcode;
	errcode = insertIntoMethods(name, params_count, method);
	if(errcode != 0) return errcode;
	return 0;
}

int ClassGraphVertex::getMethod(const string& name, unsigned int argsCount, Method*& method, bool& found) {
	NameToArgCountToMethodMap::iterator methodI = methods.find(name);
	found = false;
	if(methodI == methods.end()) {
		return 0;
	}
	ArgsCountToMethodMap* map = (*methodI).second;
	ArgsCountToMethodMap::iterator mI = map->find(argsCount);
	if(mI == map->end()) {
		return 0;
	}
	method = (*mI).second;
	found = true;
	return 0;
}

int ClassGraphVertex::initNotGraphProperties(ObjectPointer *optr, Transaction *&tr, QueryExecutor *qe) {
	int errcode = 0;
	DataValue* dv = optr->getValue();
	name = optr->getName();
	if(dv->getSubtype() != Store::Class) {
		return 1;//spodziewana klasa, otrzymano cos innego
	}
	vector<LogicalID*>* inner_vec = dv->getVector();
	for (unsigned int i=0; i < inner_vec->size(); i++) {

		ObjectPointer *inner_optr;
		errcode = tr->getObjectPointer (inner_vec->at(i), Store::Read, inner_optr, false);
		if (errcode != 0) {
			return ClassGraph::trErrorOccur(qe, "[QE] Error in getObjectPointer.", errcode);
		}
		if (inner_optr->getName() == QE_INVARIANT_BIND_NAME) {
			invariant = (inner_optr->getValue())->getString();
		}
		if (inner_optr->getName() == QE_METHOD_BIND_NAME) {
			errcode = putMethod( (inner_optr->getValue())->getPointer(), tr, qe );
			if(errcode != 0) return errcode;
		}
		if (inner_optr->getName() == QE_FIELD_BIND_NAME) {
			fields.insert( (inner_optr->getValue())->getString() );
		}
		if (inner_optr->getName() == QE_STATIC_FIELD_BIND_NAME) {
			staticFields.insert( (inner_optr->getValue())->getString() );
		}
	}
	return 0;
}

int ClassGraph::findMethod(string name, unsigned int argsCount, SetOfLids* classesToSearch, Method*& method, bool& found, LogicalID* classLidToStartSearching, SearchPhase searchPhase, LogicalID*& bindClassLid) {
	int errcode = 0;
	for(SetOfLids::iterator i = classesToSearch->begin(); i != classesToSearch->end(); ++i) {
		if(vertexExist(*i)) {
			ClassGraphVertex* cgv;
			errcode = getVertex(*i, cgv);
			if(errcode != 0) return errcode;
			if(searchPhase == SKIP_ONE_STEP) {
				findMethod(name, argsCount, &(cgv->extends), method, found, classLidToStartSearching, SEARCHING, bindClassLid);
			} else if (searchPhase == SEARCHING_START_CLASS && !((*classLidToStartSearching) == (*(*i)))) {
				findMethod(name, argsCount, &(cgv->extends), method, found, classLidToStartSearching, searchPhase, bindClassLid);
			} else {
				errcode = cgv->getMethod(name,argsCount, method, found);
				if(errcode != 0 ) return errcode;
				if(found) {
					bindClassLid = (*i);
					return 0;
				}
				findMethod(name, argsCount, &(cgv->extends), method, found, classLidToStartSearching, SEARCHING, bindClassLid);
			}
			if(found) return 0;
		}  
	}
	return 0;
}

int ClassGraph::staticFieldExist(const string& extName, bool& fieldExist) {
	fieldExist = false;
	bool isExtName = false;
	string className = firstPartFromExtName(extName, isExtName);
	if(!isExtName) {
		return 0;
	}
	ClassGraphVertex* cgv;
	bool classExist;
	int errcode = getClassVertexByName(className, cgv, classExist);
	if(errcode != 0) return errcode;
	if(!classExist) {
		return 1;//TODO: no class def found
	}
	string staticField = lastPartFromExtName(extName, isExtName);
	fieldExist = cgv->hasStaticField(staticField);
	return 0;
}

int ClassGraph::getClassVertexByName(const string& className, ClassGraphVertex*& cgv, bool& exist) {
	exist = true;
	if(!classExist(className)) {
		exist = false;
		return 0;
	}
	LogicalID* classLid;
	int errcode = getClassLidByName(className, classLid);
	if(errcode != 0) return errcode;
	return getVertex(classLid, cgv);
}

bool ClassGraph::isExtName(const string& extName) {
	return extName.find_first_of(QE_NAMES_SEPARATOR) != string::npos;
}

string ClassGraph::lastPartFromExtName(const string& extName, bool& isExtName) {
	string::size_type separatorPos = extName.find_last_of(QE_NAMES_SEPARATOR);
	isExtName = (separatorPos != string::npos);
	if(!isExtName) return "";
	return extName.substr(separatorPos + separatorLen - 1);
}

string ClassGraph::firstPartFromExtName(const string& extName, bool& isExtName) {
	string::size_type separatorPos = extName.find_first_of(QE_NAMES_SEPARATOR);
	isExtName = (separatorPos != string::npos);
	if(!isExtName) return "";
	return extName.substr(0, separatorPos);
}

int ClassGraph::findMethod(string name, unsigned int argsCount, SetOfLids* classesToSearch, Method*& method, bool& found, LogicalID* actualBindClassLid, LogicalID*& bindClassLid) {
	/*string::size_type firstSeparatorPos = name.find_first_of(QE_NAMES_SEPARATOR);
	if(firstSeparatorPos == string::npos) {
		return findMethod(name, argsCount, classesToSearch, method, found, NULL, SEARCHING, bindClassLid);
	}*/
	bool isExtName = false;
	string className = firstPartFromExtName(name, isExtName);
	if(!isExtName) {
		return findMethod(name, argsCount, classesToSearch, method, found, NULL, SEARCHING, bindClassLid);
	}
	//Ponizej wywolywanie metody za pomoca super:: lub nadklasa::,
	//wiec nalezy zaczac szukanie wzwyz od aktualnie zbindowanej klasy (actualBindClassLid)
	/*string className = name.substr(0, firstSeparatorPos);
	string methodName = name.substr(name.find_last_of(QE_NAMES_SEPARATOR) + separatorLen - 1);*/
	string methodName = lastPartFromExtName(name, isExtName);
	SetOfLids* newClassesToSearch = classesToSearch;
	SetOfLids oneClassToSearch;
	if(actualBindClassLid != NULL)
	{
		oneClassToSearch.insert(actualBindClassLid);
		newClassesToSearch = &oneClassToSearch;
	}
	if(className == QE_SUPER_KEYWORD) {
		//Uwaga: mozna wywolac wprost na obiekcie super::metoda(),
		//wtedy wykonanie zacznie sie od nadklasy,
		//jesli mialoby tak nie byc to tu trzeba zglosic blad.
		return findMethod(methodName, argsCount, newClassesToSearch, method, found, NULL, SKIP_ONE_STEP, bindClassLid);		
	}
	if(!classExist(className)) {
		return 1;//TODO blad "class def not found"
	}
	LogicalID* classLid;
	int errcode = getClassLidByName(className, classLid);
	if(errcode != 0) return errcode;
	return findMethod(methodName, argsCount, newClassesToSearch, method, found, classLid, SEARCHING_START_CLASS, bindClassLid);
}

int ClassGraph::findMethod(string name, unsigned int argsCount, SetOfLids* classesToSearch, string &code, vector<string> &params, int& founded, LogicalID* actualBindClassLid, LogicalID*& bindClassLid) {
	int errcode = 0;
	Method* method;
	bool found = false;
	errcode = findMethod(name, argsCount, classesToSearch, method, found, actualBindClassLid, bindClassLid);
	if(errcode != 0) return errcode;
	if(!found) return 0;
	founded++;
	code = method->code;
	params = method->params;
	return 0;
}

int ClassGraph::invariantToCohesiveSubgraph(string& invariantName, Transaction *&tr, QueryExecutor *qe) {
	vector<LogicalID*>* lids;
	int errcode = tr->getClassesLIDByInvariant(invariantName, lids);
	if(errcode != 0) {
		return trErrorOccur(qe,  "[QE] Error in geting class by invariant.", errcode );
	}
	for(vector<LogicalID*>::iterator i = lids->begin(); i != lids->end(); i++) {
		//(*ec) << "^^^^^^^^^^^^^^^ ------------ " << (*i)->toString();
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

void ClassGraph::putToNameIndex(string& className, LogicalID* classLid) {
	nameIndex[className] = classLid;
}

void ClassGraph::removeFromNameIndex(string& className) {
	nameIndex.erase(className);
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
		MapOfClassVertices::iterator cgvI = classGraph.find(*i);
		if(cgvI != classGraph.end()) {
			(*cgvI).second->removeSubclass(lid);
		}
	}
}

void ClassGraph::removeFromExtends(SetOfLids* subclasses, LogicalID* lid) {
	for(SetOfLids::iterator i = subclasses->begin(); i != subclasses->end(); ++i) {
		MapOfClassVertices::iterator cgvI = classGraph.find(*i);
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
	
	removeFromNameIndex((*cgvI).second->name);
	removeFromInvariant(lid, (*cgvI).second->invariant);
	removeFromSubclasses(&((*cgvI).second->extends), lid);
	removeFromExtends(&((*cgvI).second->subclasses), lid);
	
	LogicalID* lidToDel = (*cgvI).first;
	
	delete (*cgvI).second;
	classGraph.erase(cgvI);
	delete lidToDel;
	return 0;
}

int ClassGraph::lidToClassGraphLid(LogicalID* lidFrom, LogicalID*& lidTo) {
	MapOfClassVertices::iterator cgvI = classGraph.find(lidFrom);
	if(cgvI == classGraph.end()) {
		lidTo = NULL;
		return 0;
	}
	lidTo = (*cgvI).first;
	return 0;
}

int ClassGraph::addClass(ObjectPointer *optr, Transaction *&tr, QueryExecutor *qe) {
	LogicalID* newLid = optr->getLogicalID()->clone();
	DataValue* dv = optr->getValue();
	ClassGraphVertex* cgv = new ClassGraphVertex();
	int errcode = cgv->initNotGraphProperties(optr, tr, qe);
	if(errcode != 0) return errcode;
	classGraph[newLid] = cgv;
	putToInvariants(cgv->invariant, newLid);
	putToNameIndex(cgv->name, newLid);
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

int ClassGraph::updateClass(ObjectPointer *optr, Transaction *&tr, QueryExecutor *qe) {
	LogicalID* lid = optr->getLogicalID();
	int errcode = removeClass(lid);
	if(errcode != 0) return errcode;
	errcode = addClass(optr, tr, qe);
	if(errcode != 0) return errcode;
	ClassGraphVertex* cgv;
	errcode = getVertex(lid, cgv);
	if(errcode != 0) return errcode;
	LogicalID* newLid;
	errcode = lidToClassGraphLid(lid, newLid);
	if(errcode != 0) return errcode;
	SetOfLids* tmp = optr->getValue()->getSubclasses();
	for(SetOfLids::iterator i = tmp->begin(); i != tmp->end(); ++i) {
		MapOfClassVertices::iterator extCgvI = classGraph.find(*i);
		if(extCgvI == classGraph.end()) {
			removeClass(lid);
			return qe->qeErrorOccur("[ClassGraph] No class in graph.", ENoClassDefFound);
		}
		((*extCgvI).second)->addExtend(newLid);
		cgv->addSubclass((*extCgvI).first);
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

int ClassGraph::classBelongsToExtSubGraph(const MapOfClassVertices::iterator& classI, const MapOfClassVertices::iterator& subGraphI, bool& belongs) {
	if(classI == classGraph.end()) {
		return 1;//TODO: class def not found
	}
	if(subGraphI == classGraph.end()) {
		//tu raczej nie powinno byc bledu, to oznacza, ze klasa jest invalid,
		//lub, ze obiekt jest invalid (worning by sie przydal)
		//Jeszcze nalezy sprawdzic, czy tu nie moga dotrzec tylko istniejace klasy,
		//wtedy nalezy tu zglaszac jednak blad.
		return 1;//TODO: class def not found
	}
	if(classI == subGraphI) {
		belongs = true;
		return 0;
	}
	SetOfLids* superClasses =&(((*subGraphI).second)->extends);
	for(SetOfLids::iterator i = superClasses->begin(); i != superClasses->end(); ++i) {
		MapOfClassVertices::iterator newSubGraph = classGraph.find(*i);
		int errcode = classBelongsToExtSubGraph(classI, newSubGraph, belongs);
		if(errcode != 0) return errcode;
		if(belongs) return 0;
	}
	return 0;
}

int ClassGraph::checkExtendsForUpdate(const string& updatedClass, SetOfLids* extendedClasses, LogicalID*& upLid, bool& isUpdatePossible) {
	isUpdatePossible = true;
	if(!classExist(updatedClass)) {
		//TODO (sk) NoClassDefFound.
		return 1;
	}
	int errcode = getClassLidByName(updatedClass, upLid);
	if(errcode != 0) return errcode;
	bool inSubGraph = false;
	errcode = isCastAllowed(upLid, extendedClasses, inSubGraph);
	if(errcode!=0) return errcode;
	isUpdatePossible = !inSubGraph;
	/*MapOfClassVertices::iterator subGraph = classGraph.find(classLid);
	for(SetOfLids::iterator i = extendedClasses->begin(); i != extendedClasses->end(); ++i) {
		errcode = classBelongsToExtSubGraph(subGraph, classGraph.find(*i), inSubGraph);
		if(errcode != 0) return errcode;
		if(inSubGraph) {
			isUpdatePossible = false;
			return 0;
		}
	}*/
	return 0;
}

int ClassGraph::isCastAllowed(LogicalID* classLid, SetOfLids* classesToCheck, bool& includes) {
	if(classesToCheck == NULL) {
		return 0;
	}
	MapOfClassVertices::iterator classI = classGraph.find(classLid);
	for(SetOfLids::iterator i = classesToCheck->begin(); i != classesToCheck->end(); ++i) {
		MapOfClassVertices::iterator subGraph = classGraph.find(*i);
		if(subGraph == classGraph.end()) {
			continue;
		}
		int errcode = classBelongsToExtSubGraph(classI, subGraph, includes);
		if(errcode != 0) return errcode;
		if(includes) return 0;
	}
	return 0;
}

int ClassGraph::isCastAllowed(LogicalID* classLid, ObjectPointer *optr, bool& includes) {
	includes = false;
	if(optr->getValue()->getSubtype() == Store::Class ) {
		return 0;//class can't be cast
	}
	return isCastAllowed(classLid, optr->getValue()->getClassMarks(), includes);
	/*SetOfLids* classesToCheck = optr->getValue()->getClassMarks();
	if(classesToCheck == NULL) {
		return 0;
	}
	MapOfClassVertices::iterator classI = classGraph.find(classLid);
	for(SetOfLids::iterator i = classesToCheck->begin(); i != classesToCheck->end(); ++i) {
		int errcode = classBelongsToExtSubGraph(classI, classGraph.find(*i), includes);
		if(errcode != 0) return errcode;
		if(includes) return 0;
	}
	return 0;*/
}

int ClassGraph::isCastAllowed(string& className, ObjectPointer *optr, bool& includes) {
	if(!classExist(className)) {
		return 1;//TODO noclasdefffound
	}
	LogicalID* classLid;
	int errcode = getClassLidByName(className, classLid);
	if(errcode != 0) return errcode;
	return isCastAllowed(classLid, optr, includes);
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

ClassGraphVertex::~ClassGraphVertex() {
	for(NameToArgCountToMethodMap::iterator i = methods.begin(); i != methods.end(); ++i) {
		ArgsCountToMethodMap* toDel = (*i).second;
		for(ArgsCountToMethodMap::iterator j = toDel->begin(); j != toDel->end(); ++j) {
			delete ( (*j).second );
		}
		delete toDel;
	}
}

ClassGraph::ClassGraph():lazy(false) { 
	string str = QE_NAMES_SEPARATOR;
	separatorLen = str.length();
}

}
