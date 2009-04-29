#include "InterfaceMaps.h"
#include "ClassGraph.h"
#include "Errors/ErrorConsole.h"
#include "TransactionManager/Transaction.h"
#include "CommonOperations.h"

using namespace Schemas;
using namespace TManager;
using namespace CommonOperations;


InterfaceMaps::InterfaceMaps() {ec = &ErrorConsole::get_instance(EC_INTERFACE_MAPS);}

void InterfaceMaps::printAll() const
{
	debug_printf(*ec, "\n\n Interfaces in map: \n");
	TInterfaceToSchemas::const_iterator it;
	for (it = m_nameToSchema.begin(); it != m_nameToSchema.end(); ++it)
	{
		string name = (*it).first;
		Schema s = (*it).second;
		debug_printf(*ec, "\n***%s***\n", name.c_str());
		s.printAll();
	}	
	
	debug_printf(*ec, "\n InterfacesHierarchy: \n");
	TInterfaceToHierarchy::const_iterator hit;
	for (hit = m_nameToHierarchy.begin(); hit != m_nameToHierarchy.end(); ++hit)
	{
		string name = (*hit).first;
		HierarchyInfo h = (*hit).second;
		debug_printf(*ec, "\n***%s***\n", name.c_str());
		
		TStringSet parents = h.getParents();
		TStringSet children = h.getChildren();
		
		debug_printf(*ec, "\t Parents: ");
		TStringSet::iterator p;
		string totalPar;
		for (p = parents.begin(); p!=parents.end(); ++p)
		{
			string par = *p;
			totalPar += par + " ";
		}
		debug_print(*ec,  totalPar);
		
		debug_printf(*ec, "\n\t Children: ");
		string totalChild;
		for (p = children.begin(); p!=children.end(); ++p)
		{
			string child = *p;
			totalChild += child + " ";
		}
		debug_print(*ec,  totalChild);
		debug_printf(*ec, "\n");
	}
	
	debug_printf(*ec, "\n InterfacesInExtends: \n");
	TNameInHierarchy::const_iterator nit;
	for (nit = m_nameToInterfacesExtending.begin(); nit != m_nameToInterfacesExtending.end(); ++nit)
	{
		string name = (*nit).first;
		TStringSet ext = (*nit).second;
		debug_printf(*ec, "Interface %s is extended by: ", name.c_str());
		TStringSet::iterator i;
		string totalExt;
		for (i = ext.begin(); i != ext.end(); ++i)
		{
			string extName = *i;
			totalExt += extName + " ";
		}
		debug_print(*ec,  totalExt);
		debug_printf(*ec, "\n");
	}
	
	debug_printf(*ec, "\n BindMap:\n");
	m_bindMap.print(ec);
	
	debug_printf(*ec, "\n");
}

int InterfaceMaps::init()
{
	Transaction *tr;
	int errcode = (TransactionManager::getHandle())->createTransaction(-1, tr);
	if (errcode != 0) 
	{
		return errcode;
	}
	debug_printf(*ec, "InterfaceMaps::init() getting lids..\n");
	vector<LogicalID*>* interfacesLids;
	errcode = tr->getInterfacesLID(interfacesLids);
	if (errcode != 0) 
	{
		debug_printf(*ec, "InterfaceMaps::init() error\n");
		return errcode;
	}
	
	debug_printf(*ec, "InterfaceMaps::init() loadingSchemas..\n");
	errcode = loadSchemas(tr, interfacesLids);
	if (errcode != 0) 
	{
		debug_printf(*ec, "InterfaceMaps::loadSchemas() error, %d\n", errcode);
		return errcode;
	}
	errcode = tr->commit();
	if (errcode) 
	{
		return errcode;
	}
	//printAll();
	debug_printf(*ec, "InterfaceMaps::init(): loaded %d interfaces, returning\n", m_nameToSchema.size());
	return 0;
}

void InterfaceMaps::deinit()
{
	clearMaps();
}

void InterfaceMaps::clearMaps()
{
	m_nameToSchema.clear();
	m_nameToHierarchy.clear();
	m_nameToInterfacesExtending.clear();
	m_bindMap.clear();
	m_cache.clear();
}

int InterfaceMaps::loadSchemas(TManager::Transaction *tr, TLidsVector *lvec)
{
	clearMaps();
	
	int errcode;
	TLidsVector::iterator it;
	for (it = lvec->begin(); it != lvec->end(); ++it)
	{
		LogicalID *interfaceLid = *it;
		Schema *interfaceSchema = new Schema();
		errcode = Schema::interfaceFromLogicalID(interfaceLid, tr, interfaceSchema);
		if (errcode) return errcode;
		insertNewInterfaceAndPropagateHierarchy(*interfaceSchema, false, false);
		string oN = interfaceSchema->getAssociatedObjectName();
		debug_printf(*ec, "InterfaceMaps::loadSchemas: adding bind for int=%s, obj=%s\n", interfaceSchema->getName().c_str(), oN.c_str());
		errcode = addBind(oN, tr);
		if (errcode)
		{
			clearMaps();
			debug_printf(*ec, "InterfaceMaps::loadSchemas: error in FindClassBoundToInterface\n");
			return errcode;
		}
	}

	if (!checkHierarchyValidity())
	{   
		debug_printf(*ec, "InterfaceMaps::loadSchemas: error - corrupted hierarchy!\n");
		return (ErrQExecutor | EInterfaceHierarchyInvalid);
	}
	
	return 0;
}

int InterfaceMaps::addInterface(LogicalID* interfaceLid, TManager::Transaction *tr, int ct, bool checkValidity)
{
	Schema *s;
	int errcode = Schema::interfaceFromLogicalID(interfaceLid, tr, s);
	if (errcode)
	{
		debug_printf(*ec, "[InterfaceMaps::addInterface] error in interfaceFromLogicalID");
		return errcode;
	}

	ObjectPointer *optr;
	errcode = tr->getObjectPointer (interfaceLid, Store::Read, optr, false);
	if (errcode) 
	{
		debug_printf(*ec, "[InterfaceMaps::addInterface] getObjectPointer returned error\n");
		return errcode;
	}

	debug_printf(*ec, "[InterfaceMaps::addInterface] got object pointer\n");
	string interfaceName = optr->getName().c_str();
	string objectName = "";
	vector<LogicalID*>* vec = (optr->getValue())->getVector();
	for (unsigned int i=0; i < vec->size(); i++)
	{
		ObjectPointer *inner;
		errcode = tr->getObjectPointer (vec->at(i), Store::Read, inner, false);
		if (errcode)
		{
			debug_printf(*ec, "[InterfaceMaps::addInterface] - Error in getObjectPointer\n");
			return errcode;
		}
		if (inner->getName() == QE_OBJECT_NAME_BIND_NAME)
		{
			objectName = (inner->getValue())->getString();
			break;
		}
	}
	debug_printf(*ec,  "[InterfaceMaps::addInterface] interface name is %s, object name is %s", interfaceName.c_str(), objectName.c_str());
	debug_printf(*ec,  "[InterfaceMaps::addInterface] Checking if name is already used in database..");

	bool taken;
	errcode = nameTaken(interfaceName, tr, taken);
	if (errcode) return errcode;
	if (ct == CT_CREATE)
	{
		if (taken)
		{   
			debug_printf(*ec, "[InterfaceMaps::addInterface] interface name %s already in use", interfaceName.c_str());
			return (ErrQExecutor | ENotUniqueInterfaceName);
		}
		if (!objectName.empty())
		{
			errcode = nameTaken(objectName, tr, taken);
			if (errcode || taken)
			{
				if (errcode) return errcode;
				debug_printf(*ec, "[InterfaceMaps::addInterface] interface object name %s already in use", objectName.c_str());
				return (ErrQExecutor | ENotUniqueInterfaceName);		
			}		
		}
	}
	bool present = hasInterface(interfaceName);
	if (present)
	{		
		errcode = removeInterface(interfaceName, tr, false);
		if (errcode) return errcode;		
	}
	
	if ((checkValidity) && (!checkIfCanBeInserted(*s)))
	{
		debug_printf(*ec, "[InterfaceMaps::addInterface] interface %s is invalid in current hierarchy", s->getName().c_str());
		return (ErrQExecutor | ESpoilsInterfaceHierarchy);;
	}
	
	errcode = tr->addRoot(optr);
	if (errcode != 0) 
	{
		debug_print(*ec, "[InterfaceMaps::addInterface] error in addRoot");
		return errcode;
	}
	debug_printf(*ec, "[InterfaceMaps::addInterface] addRoot processed\n");

	errcode = tr->addInterface(interfaceName, objectName.c_str(), optr);
	if (errcode != 0) 
	{
		debug_print(*ec, "[InterfaceMaps::addInterface] error in tr->addInterface");
		return errcode;
	}

	debug_printf(*ec, "[InterfaceMaps::addInterface] inserting into map..\n");
	insertNewInterfaceAndPropagateHierarchy(*s, checkValidity, false);
	debug_print(*ec,  "[InterfaceMaps::addInterface] returning");
	//printAll();
	return 0;	
}

int InterfaceMaps::removeInterface(string interfaceName, TManager::Transaction *tr, bool checkValidity)
{
	debug_printf(*ec, "[QE] removing interface %s\n", interfaceName.c_str());
	vector<LogicalID*>* lids;
	int errcode = tr->getInterfacesLID(interfaceName, lids);
	LogicalID *oldLid = lids->at(0);
	ObjectPointer *p;
	errcode = tr->getObjectPointer (oldLid, Store::Write, p, true);
	if (errcode || !p) 
	{
		debug_printf(*ec, "[InterfaceMaps::removeInterface] getObjectPointer returned error\n");
		return errcode;
	}
		
	return removeInterface(interfaceName, tr, p, checkValidity);
}

int InterfaceMaps::removeInterface(string interfaceName, TManager::Transaction *tr, ObjectPointer *p, bool checkValidity)
{
	debug_printf(*ec, "[QE] removing interface %s\n", interfaceName.c_str());
	int errcode = tr->removeInterface(p);
	if (errcode) return errcode;
	if (p->getIsRoot())
	{
		errcode = tr->removeRoot(p);
		if (errcode) return errcode;
	}
	errcode = tr->deleteObject(p);
	if (errcode) return errcode;
	errcode = removeInterfaceFromMaps(interfaceName, checkValidity);
	if (errcode) return errcode;
	return 0;
}

int InterfaceMaps::bindInterface(string interfaceOrObjectName, string implementationOrObjectName, TManager::Transaction *tr, bool &matches)
{
	matches = false;
	
	//find interface name
	Schema *interfaceSchema;
	bool exists = false;
	Schema::getInterfaceForInterfaceOrObjectName(interfaceOrObjectName, exists, interfaceSchema);
	if (!exists)
	{
		debug_printf(*ec, "[InterfaceMaps::bindInterface] no interface or object name %s in db", interfaceOrObjectName.c_str());
		return (ErrQExecutor | ENoInterfaceFound);
	}
	string interfaceName = interfaceSchema->getName();
	debug_printf(*ec, "[InterfaceMaps::bindInterface] Got name: %s\n", interfaceName.c_str());
	
	//find implementation name
	Schema *implementationSchema;
	int implementationType;
	exists = false;
	int errcode = Schema::getImplementationForName(implementationOrObjectName, tr, exists, implementationType, implementationSchema);
	if (errcode) return errcode;
	if (!exists)
	{
		debug_printf(*ec, "[InterfaceMaps::bindInterface] no implementation or object name %s in db", implementationOrObjectName.c_str());
		return (ErrQExecutor | ENoImplementationFound);
	}
	string implementationName = implementationSchema->getName();
	debug_printf(*ec, "[InterfaceMaps::bindInterface] Got implementation name: %s\n", implementationName.c_str());

	//Check if implementation fits interface	    
	if (implementationType == BIND_VIEW)
	{
		debug_printf(*ec, "[InterfaceMaps::bindInterface] Bind to a view - matching can't be easily checked, so it won't be\n");
		matches = true;
	}
	else
	{			
		errcode = Schema::interfaceMatchesImplementation(interfaceName, implementationName, tr, implementationType, matches);
		if (errcode) 
		{
			debug_printf(*ec, "[InterfaceMaps::bindInterface] - error in interfaceMatchesImplementation: %s, %s\n", interfaceName.c_str(), implementationName.c_str());
			return errcode;
		}
		string resS = matches ? "fits": "does not fit";
		debug_printf(*ec, "[InterfaceMaps::bindInterface] - interface %s %s implementation %s\n",interfaceName.c_str(),resS.c_str(),implementationName.c_str());
	}
	
	//Bind interface
	if (matches)
	{
		string implementationObjectName = implementationSchema->getAssociatedObjectName();
		string interfaceObjectName = interfaceSchema->getAssociatedObjectName();
		if (interfaceObjectName.empty())
		{
			debug_printf(*ec, "[InterfaceMaps::bindInterface] - cannot bind interface %s: it has no associated object name\n", interfaceName.c_str()); 
			return (ErrQExecutor | ECannotBindNoObjectName);
		}
		else
		{
			errcode = tr->bindInterface(interfaceName, implementationName);
			if (errcode) 
			{
				debug_printf(*ec, "[InterfaceMaps::bindInterface] - error in bindInterface\n");
				return (ErrTManager | EOther);
			}
			addBind(interfaceName, implementationName, implementationObjectName, implementationType);
		}
	}
	OuterSchemas::Instance().revalidateAllSchemasUsingName(interfaceName, tr);
	return 0;
}

string InterfaceMaps::getObjectNameForInterface(string i, bool &found) const
{
	string out;
	found = false;
	if (i.empty()) return out;
	TInterfaceToSchemas::const_iterator it = m_nameToSchema.find(i);
	if (it != m_nameToSchema.end())
	{
		found = true;
		out = (*it).second.getAssociatedObjectName();
	}
	return out;
}

string InterfaceMaps::getInterfaceNameForObject(string o, bool &found) const
{
	string out;
	found = false;
	if (o.empty()) return out;
	TDict::const_iterator it = m_objNameToName.find(o);
	if (it != m_objNameToName.end())
	{
		found = true;
		out = (*it).second;
	}
	return out;
}

bool InterfaceMaps::isObjectNameBound(string o) const
{
	if (o.empty()) return false;
	bool f;
	string i = getInterfaceNameForObject(o, f);
	if (!f) return false;
	return m_bindMap.hasBind(i);		
}

int InterfaceMaps::addBind(string objectName, TManager::Transaction *tr)
{
	string iName, impName, impObjName;	
	int t;
	bool found;
	int errcode = Matcher::FindImpBoundToInterface(objectName, tr, iName, impName, impObjName, found, t);
	if (errcode) return errcode;
	if (found)
		addBind(iName, impName, impObjName, t);
	return 0;
}

void InterfaceMaps::addBind(string i, string imp, string impObj, int type)
{
	m_bindMap.addBind(i, imp, impObj, static_cast<BindType>(type));
}

ImplementationInfo InterfaceMaps::getImplementationForInterface(string name, bool &found, bool final) const
{	
	return m_bindMap.getImpForInterface(name, found, final);
}

ImplementationInfo InterfaceMaps::getImplementationForObject(string name, bool &found, bool final) const
{	
	ImplementationInfo iI;
	string iName = getInterfaceNameForObject(name, found);
	if (!found) return iI;
	return m_bindMap.getImpForInterface(iName, found, final);
}

//TODO - see below
void InterfaceMaps::implementationUpdated(string implementationName, string objectName, int type, TManager::Transaction *tr)
{
	set<string> iShowing = m_bindMap.interfacesShowingImplementation(implementationName);
	for (set<string>::iterator it = iShowing.begin(); it != iShowing.end(); ++it)
	{   //For every interface bound to implementation (which was just updated), check if it still matches this implementation 
		string interfaceName = (*it);
		bool matches;
		Schema::interfaceMatchesImplementation(interfaceName, implementationName, tr, type, matches);	
		if (!matches)
		{   //if not, remove bind from the map and re-validate (invalidate in this case) schemas that are using this interface
			m_bindMap.removeBind(interfaceName);
			OuterSchemas::Instance().revalidateAllSchemasUsingName(interfaceName, tr);
		}
	}
	//change object name associated with implementation name in bind map to objectName (can be the same name) 
	m_bindMap.changeObjectNameForImplementation(implementationName, objectName);
}

//TODO - A shows B, B shows C, C removed -> invalidate schemas using A too! (perhaps handle it in Schemas)
void InterfaceMaps::implementationRemoved(string implementationName, TManager::Transaction *tr) 
{   //all schemas using implementationName or any interfaceName bound to it should be re-validated;
	OuterSchemas::Instance().revalidateAllSchemasUsingName(implementationName, tr);
	set<string> iShowing = m_bindMap.interfacesShowingImplementation(implementationName);
	for (set<string>::iterator it = iShowing.begin(); it != iShowing.end(); ++it)
	{ 
		string interfaceName = (*it);
		m_bindMap.removeBind(interfaceName);
		OuterSchemas::Instance().revalidateAllSchemasUsingName(interfaceName, tr);
	}
	m_bindMap.removeEntriesForImplementation(implementationName);
}

bool InterfaceMaps::checkDerivationValidity(string baseName, TStringSet forbiddenNames, TStringSet *&namesAlreadyCheckedUpwardsP) const
{
	if (forbiddenNames.find(baseName) != forbiddenNames.end())
		return false;
	forbiddenNames.insert(baseName);
	
	TInterfaceToHierarchy::const_iterator innIt = m_nameToHierarchy.find(baseName);
	if (innIt == m_nameToHierarchy.end())
		return false;
	
	TStringSet curPars = (*innIt).second.getParents();
	for (TStringSet::iterator itP = curPars.begin(); itP != curPars.end(); ++itP)
	{
		string parentName = *itP;
		bool validBranch = checkDerivationValidity(parentName, forbiddenNames, namesAlreadyCheckedUpwardsP);
		if (!validBranch)
			return false; //derivation loop somewhere
		debug_printf(*ec, "[InterfaceMaps::checkDerivationValidity] name %s checked to have valid parents", parentName.c_str());
		namesAlreadyCheckedUpwardsP->insert(parentName);
	}	
	
	return true;
}

bool InterfaceMaps::checkHierarchyValidity() const
{
	TStringSet namesAlreadyCheckedUpwards;
	TStringSet *namesAlreadyCheckedUpwardsP = &namesAlreadyCheckedUpwards;
	TInterfaceToHierarchy::const_iterator it;
	for (it = m_nameToHierarchy.begin(); it != m_nameToHierarchy.end(); ++it)
	{
		string name = (*it).first;
		//debug_printf(*ec, "[InterfaceMaps::checkHierarchyValidity] checking name %s", name.c_str());
		if (namesAlreadyCheckedUpwards.find(name) != namesAlreadyCheckedUpwards.end())
			continue;   //this name was already checked for valid parents		
		//debug_printf(*ec, "[InterfaceMaps::checkHierarchyValidity] name %s not checked before", name.c_str());

		if (!hasInterface(name))
			return false;   //"ghost" interface name in extends  
		//debug_printf(*ec, "[InterfaceMaps::checkHierarchyValidity] name %s has interface", name.c_str());
		
		TStringSet forbiddenNames; //empty
		
		if (!checkDerivationValidity(name, forbiddenNames, namesAlreadyCheckedUpwardsP))
			return false; //found loop
	}
	return true;	
}


bool InterfaceMaps::insertNewInterfaceAndPropagateHierarchy(Schema interfaceSchema, bool checkValidity, bool tryOnly)
{
	TNameInHierarchy nihCpy(m_nameToInterfacesExtending);
	TInterfaceToHierarchy ithCpy(m_nameToHierarchy);
	
	string interfaceName = interfaceSchema.getName();
	debug_printf(*ec, "insertNewInterfaceAndPropagateHierarchy starts with interfaceName = %s\n", interfaceName.c_str());
	TSupers supers = interfaceSchema.getSupers();	
	HierarchyInfo hI;
	
	if (m_nameToInterfacesExtending.find(interfaceName) != m_nameToInterfacesExtending.end())
	{
		TStringSet intsToSub = m_nameToInterfacesExtending[interfaceName];
		TStringSet::iterator it;
		for (it = intsToSub.begin(); it != intsToSub.end(); ++it)
		{
			string child = *it;
			hI.addChild(child);
		}		
	}
	
	TSupers::iterator it;
	for (it = supers.begin(); it != supers.end(); ++it)
	{
		string parent = *it;
		hI.addParent(parent);
		TStringSet set = m_nameToInterfacesExtending[parent];
		set.insert(interfaceName);
		m_nameToInterfacesExtending[parent] = set;
		if (m_nameToHierarchy.find(parent) != m_nameToHierarchy.end())
		{
			m_nameToHierarchy[parent].addChild(interfaceName);		
		}
	}
	
	m_nameToHierarchy[interfaceName] = hI;
	m_nameToSchema[interfaceName] = interfaceSchema;
	
	bool valid = checkHierarchyValidity();
	if ((checkValidity && !valid) || (tryOnly))
	{
		debug_printf(*ec, "insertNewInterfaceAndPropagateHierarchy: reversing changes\n");
		m_nameToSchema.erase(interfaceName);
		m_nameToHierarchy = ithCpy;
		m_nameToInterfacesExtending = nihCpy;
		if (!valid)
			return false;  //Invalid interface, not inserted
		else 
			return true;
	}
		
	m_cache.clear();
	string objName = interfaceSchema.getAssociatedObjectName();
	m_objNameToName[objName] = interfaceName;
	return true;
}

int InterfaceMaps::removeInterfaceFromMaps(string interfaceName, bool checkValidity)
{	
	if (!hasInterface(interfaceName))
	{   //No such interface!
		return 0;
	}
		
	TNameInHierarchy nihCpy(m_nameToInterfacesExtending);
	TInterfaceToHierarchy ithCpy(m_nameToHierarchy);
	Schema s = m_nameToSchema[interfaceName];
	
	m_nameToSchema.erase(interfaceName);
	TStringSet parents = m_nameToHierarchy[interfaceName].getParents();
	TStringSet::iterator it;
	for (it = parents.begin(); it != parents.end(); ++it)
	{
		string par = *it;
		m_nameToHierarchy[par].removeChild(interfaceName);		
	}
	TStringSet children = m_nameToHierarchy[interfaceName].getChildren();
	for (it = children.begin(); it != children.end(); ++it)
	{
		string child = *it;
		TStringSet extendingChild = m_nameToInterfacesExtending[child];
		extendingChild.erase(interfaceName);
		m_nameToInterfacesExtending[child] = extendingChild;
	}
	
	m_nameToHierarchy.erase(interfaceName);
	
	if (checkValidity && !checkHierarchyValidity())
	{
		m_nameToSchema[interfaceName] = s;
		m_nameToHierarchy = ithCpy;
		m_nameToInterfacesExtending = nihCpy;
		return (ErrQExecutor | ESpoilsInterfaceHierarchy); //Removal illegal, not removed
	}
	
	string oN = s.getAssociatedObjectName();
	m_bindMap.removeBind(interfaceName);
	m_objNameToName.erase(oN);
	m_cache.clear();
	

	return 0;
}
			

TStringSet InterfaceMaps::getAllExt(string interfaceName) const
{
	TStringSet allExt;
	TInterfaceToHierarchy::const_iterator it = m_nameToHierarchy.find(interfaceName);
	if (it == m_nameToHierarchy.end())
	{   //Should not happen!
		return allExt;
	}
	allExt = (*it).second.getParents();
	vector<string> workingVec;
	workingVec.insert(workingVec.begin(), allExt.begin(), allExt.end());
	while (!workingVec.empty())
	{
		string name = workingVec.back();
		workingVec.pop_back();
		
		if (!hasInterface(name))
			continue;   //Should not happen
		
		it = m_nameToHierarchy.find(name);
		if (it == m_nameToHierarchy.end()) return allExt;   //Should not happen
		
		TStringSet curParents = (*it).second.getParents();
		if (curParents.empty())
			continue;   //Top of hierarchy reached
		
		allExt.insert(curParents.begin(), curParents.end());
		workingVec.insert(workingVec.end(), curParents.begin(), curParents.end());
	}
	return allExt;
}

TFields InterfaceMaps::getFieldsIncludingDerived(string interfaceName, bool cache) const
{
	if (cache && m_cache.hasFieldsFor(interfaceName))
	{
		return m_cache.getFieldsFor(interfaceName);
	}
	
	TFields allFields;
	TInterfaceToSchemas::const_iterator cit = m_nameToSchema.find(interfaceName);
	if (cit == m_nameToSchema.end()) return allFields; //should not happen
	allFields = (*cit).second.getFields();
	
	TStringSet allExt = getAllExt(interfaceName);
	TStringSet::iterator it;
	for (it = allExt.begin(); it != allExt.end(); ++it)
	{
		string name = *it;
		cit = m_nameToSchema.find(name);
		if (cit == m_nameToSchema.end()) return allFields; //should not happen
		TFields curFields = (*cit).second.getFields();
		allFields.insert(allFields.end(), curFields.begin(), curFields.end());
	}
	//TODO - check for repetitions?
	
	if (cache)
	{//cache results
		m_cache.addResult(interfaceName, allFields);
	}
	
	return allFields;
}

Schemas::TMethods InterfaceMaps::getMethodsIncludingDerived(string interfaceName, bool cache) const
{
	if (cache && m_cache.hasMethodsFor(interfaceName))
	{
		return m_cache.getMethodsFor(interfaceName);
	}
	
	Schemas::TMethods allMethods;
	TInterfaceToSchemas::const_iterator cit = m_nameToSchema.find(interfaceName);
	if (cit == m_nameToSchema.end()) return allMethods; //should not happen
	allMethods = (*cit).second.getMethods();
	
	TStringSet allExt = getAllExt(interfaceName);
	TStringSet::iterator it;
	for (it = allExt.begin(); it != allExt.end(); ++it)
	{
		string name = *it;
		cit = m_nameToSchema.find(name);
		if (cit == m_nameToSchema.end()) return allMethods; //should not happen
		Schemas::TMethods curMethods = (*cit).second.getMethods();
		allMethods.insert(allMethods.end(), curMethods.begin(), curMethods.end());
	}
	//TODO - check for repetitions?
	
	if (cache)
	{//cache results
		m_cache.addResult(interfaceName, allMethods);
	}
	
	return allMethods;
}

TStringSet InterfaceMaps::getAllFieldNamesAndCheckBind(string interface, bool &filterOut) const
{
		filterOut = false;
		TFields fieldsVisible;
		TStringSet namesVisible;
		if (!interface.empty())
		{
			if (isInterfaceBound(interface))
			{
				filterOut = true;
				fieldsVisible = getFieldsIncludingDerived(interface);
				TFields::iterator it;
				for (it = fieldsVisible.begin(); it != fieldsVisible.end(); ++it)
				{
					string name = (*it)->getName();
					namesVisible.insert(name);
					debug_printf(*ec, "[InterfaceMaps::getAllFieldNamesAndCheckBind] got name: %s", name.c_str());
				}
			}
		}
		return namesVisible;
}

bool InterfaceMaps::interfaceHasMethod(string interfaceName, string methodName, int args) const
{
	Schemas::TMethods allMethods = getMethodsIncludingDerived(interfaceName);
	Schemas::TMethods::iterator it;
	for (it = allMethods.begin(); it != allMethods.end(); ++it)
	{
		Schemas::Method *m = *it;
		if ((m->getName() == methodName) && (m->getParamsCount() == args))
			return true;
	}
	return false;
}

Schema InterfaceMaps::getSchemaForInterface(string interfaceName) const 
{
	Schema s;
	TInterfaceToSchemas::const_iterator it = m_nameToSchema.find(interfaceName);
	if (it != m_nameToSchema.end())
		s = (*it).second;
	return s;
}

