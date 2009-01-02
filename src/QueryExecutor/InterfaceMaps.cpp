#include "InterfaceMaps.h"
#include "ClassGraph.h"
#include "Errors/ErrorConsole.h"
#include "TransactionManager/Transaction.h"

using namespace Schemas;
using namespace TManager;


InterfaceMaps::InterfaceMaps() {ec = &ErrorConsole::get_instance(EC_INTERFACE_MAPS);}

void InterfaceMaps::printAll() const
{
	ec->printf("\n\n Interfaces in map: \n");
	TInterfaceToSchemas::const_iterator it;
	for (it = m_nameToSchema.begin(); it != m_nameToSchema.end(); ++it)
	{
		string name = (*it).first;
		Schema s = (*it).second;
		ec->printf("\n***%s***\n", name.c_str()); 		
		s.printAll(ec);
	}	
	
	ec->printf("\n InterfacesHierarchy: \n");
	TInterfaceToHierarchy::const_iterator hit;
	for (hit = m_nameToHierarchy.begin(); hit != m_nameToHierarchy.end(); ++hit)
	{
		string name = (*hit).first;
		HierarchyInfo h = (*hit).second;
		ec->printf("\n***%s***\n", name.c_str());
		
		TStringSet parents = h.getParents();
		TStringSet children = h.getChildren();
		
		ec->printf("\t Parents: ");
		TStringSet::iterator p;
		string totalPar;
		for (p = parents.begin(); p!=parents.end(); ++p)
		{
			string par = *p;
			totalPar += par + " ";
		}
		*ec << totalPar;	
		
		ec->printf("\n\t Children: ");
		string totalChild;
		for (p = children.begin(); p!=children.end(); ++p)
		{
			string child = *p;
			totalChild += child + " ";
		}
		*ec << totalChild;
		ec->printf("\n");
	}
	
	ec->printf("\n InterfacesInExtends: \n");
	TNameInHierarchy::const_iterator nit;
	for (nit = m_nameToInterfacesExtending.begin(); nit != m_nameToInterfacesExtending.end(); ++nit)
	{
		string name = (*nit).first;
		TStringSet ext = (*nit).second;
		ec->printf("Interface %s is extended by: ", name.c_str());
		TStringSet::iterator i;
		string totalExt;
		for (i = ext.begin(); i != ext.end(); ++i)
		{
			string extName = *i;
			totalExt += extName + " ";
		}
		*ec << totalExt;
		ec->printf("\n");
	}
	
	ec->printf("\n BindMap:\n");
	m_bindMap.print(ec);
	
	ec->printf("\n");
}

int InterfaceMaps::init()
{
	Transaction *tr;
	int errcode = (TransactionManager::getHandle())->createTransaction(-1, tr);
	if (errcode != 0) 
	{
		return errcode;
	}
	ec->printf("InterfaceMaps::init() getting lids..\n");
	vector<LogicalID*>* interfacesLids;
	errcode = tr->getInterfacesLID(interfacesLids);
	if (errcode != 0) 
	{
		ec->printf("InterfaceMaps::init() error\n");
		return errcode;
	}
	
	ec->printf("InterfaceMaps::init() loadingSchemas..\n");
	errcode = loadSchemas(tr, interfacesLids);
	if (errcode != 0) 
	{
		ec->printf("InterfaceMaps::loadSchemas() error, %d\n", errcode);
		return errcode;
	}
	errcode = tr->commit();
	if (errcode) 
	{
		return errcode;
	}
	//printAll();
	ec->printf("InterfaceMaps::init(): loaded %d interfaces, returning\n", m_nameToSchema.size());
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
		ec->printf("InterfaceMaps::loadSchemas: adding bind for int=%s, obj=%s\n", interfaceSchema->getName().c_str(), oN.c_str());
		errcode = addBind(oN, tr);
		if (errcode)
		{
			clearMaps();
			ec->printf("InterfaceMaps::loadSchemas: error in FindClassBoundToInterface\n");
			return errcode;
		}
	}

	if (!checkHierarchyValidity())
	{   //Interfaces corrupted - Loop in hierarchy! (TODO)
		ec->printf("InterfaceMaps::loadSchemas: error - corrupted hierarchy!\n");
		return -1;
	}
	
	return 0;
}

string InterfaceMaps::getObjectNameForInterface(string i, bool &found) const
{
	string out;
	found = false;
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

/*
void InterfaceMaps::getInterfaceBindForObjName(string oName, string& iName, string& impName, string &impObjName, int &type, bool &found, bool final) const
{
	iName = getInterfaceNameForObject(oName, found);
	if (!found) return;
	ImplementationInfo i = getImplementationForInterface(iName, found, final);
	impName = i.getName();
	impObjName = i.getObjectName();
	type = i.getType();
}
*/

/*
void InterfaceMaps::getClassBindForInterface(string interfaceName, LogicalID*& classGraphLid) const
{
	ec->printf("InterfaceMaps::getClassBindForInterface called for %s\n", interfaceName.c_str());
	bool found;
	ImplementationInfo i = m_bindMap.getImpForInterface(interfaceName, found, true);
	if (!found || i.getType() != BIND_CLASS) return;
	string cName = i.getName();
	QExecutor::ClassGraph *cg;
	int errcode = QExecutor::ClassGraph::getHandle(cg);
	if (errcode) {ec->printf("InterfaceMaps::getClassBindForInterface: error - no handle\n"); return;}
	cg->getClassLidByName(cName, classGraphLid);
	if ((classGraphLid) && (!(cg->vertexExist(classGraphLid)))) classGraphLid = NULL;
}
*/

//TODO - see below
void InterfaceMaps::implementationUpdated(string implementationName, string objectName, int type, TManager::Transaction *tr)
{
	set<string> iShowing = m_bindMap.interfacesShowingImplementation(implementationName);
	for (set<string>::iterator it = iShowing.begin(); it != iShowing.end(); ++it)
	{   //For every interface bound to implementation (which was just updated), check if it still matches this implementation 
		string interfaceName = (*it);
		bool matches;
		int errcode = Schema::interfaceMatchesImplementation(interfaceName, implementationName, tr, type, matches);	
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

bool InterfaceMaps::checkHierarchyValidity() const
{
	TStringSet namesAlreadyCheckedUpwards;
	TInterfaceToHierarchy::const_iterator it;
	for (it = m_nameToHierarchy.begin(); it != m_nameToHierarchy.end(); ++it)
	{
		string name = (*it).first;
		if (namesAlreadyCheckedUpwards.find(name) != namesAlreadyCheckedUpwards.end())
			continue;   //this name was already checked for valid parents		
		
		TStringSet fromThisNameIncluding;
		vector<string> workingVec;
		workingVec.push_back(name);
		while (!workingVec.empty())
		{
			string pName = workingVec.back();
			if (!hasInterface(pName))
				return false;   //"ghost" interface name in extends  
			workingVec.pop_back();
			if (fromThisNameIncluding.find(pName) != fromThisNameIncluding.end())
				return false;   //loop!
			fromThisNameIncluding.insert(pName);
			TInterfaceToHierarchy::const_iterator innIt = m_nameToHierarchy.find(pName);
			TStringSet curPars = (*innIt).second.getParents();
			workingVec.insert(workingVec.begin(), curPars.begin(), curPars.end());			
		}
		namesAlreadyCheckedUpwards.insert(fromThisNameIncluding.begin(), fromThisNameIncluding.end());
	}
	return true;	
}


bool InterfaceMaps::insertNewInterfaceAndPropagateHierarchy(Schema interfaceSchema, bool checkValidity, bool tryOnly)
{
	TNameInHierarchy nihCpy(m_nameToInterfacesExtending);
	TInterfaceToHierarchy ithCpy(m_nameToHierarchy);
	
	string interfaceName = interfaceSchema.getName();
	ec->printf("insertNewInterfaceAndPropagateHierarchy starts with interfaceName = %s\n", interfaceName.c_str());
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
		ec->printf("insertNewInterfaceAndPropagateHierarchy: reversing changes\n");
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

int InterfaceMaps::removeInterface(string interfaceName, bool checkValidity)
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
		return -1;  //Removal illegal, not removed
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
	if (it == m_nameToHierarchy.end());
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

