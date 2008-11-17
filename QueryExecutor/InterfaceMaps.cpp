#include "InterfaceMaps.h"
#include "ClassGraph.h"
#include "Errors/ErrorConsole.h"
#include "TransactionManager/Transaction.h"

using namespace Schemas;
using namespace TManager;


InterfaceMaps::InterfaceMaps() {ec = new ErrorConsole("InterfaceMaps");}

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
	//na razie nie dziala dobrze
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

int InterfaceMaps::addBind(string objectName, TManager::Transaction *tr)
{
	string iName, cName, invName;	
	int errcode = Matcher::FindClassBoundToInterface(objectName, tr, iName, cName, invName);
	if (errcode) return errcode;
	addBind(iName, objectName, cName, invName);
	return 0;
}

void InterfaceMaps::addBind(string i, string o, string c, string inv)
{
	ec->printf("addBind: obj = %s, int = %s\n", o.c_str(), i.c_str());
	m_bindMap.addBind(i, o, c, inv);
}

void InterfaceMaps::getInterfaceBindForObjName(string oName, string& iName, string& cName, string &invName) const
{
	iName = m_bindMap.getIntForObj(oName);
	cName = m_bindMap.getClassForInt(iName);
	invName = m_bindMap.getInvForInt(iName);
}

void InterfaceMaps::getClassBindForInterface(string interfaceName, LogicalID*& classGraphLid) const
{
	ec->printf("InterfaceMaps::getClassBindForInterface called for %s\n", interfaceName.c_str());
	string cName = m_bindMap.getClassForInt(interfaceName);
	if (cName.empty()) return;
	QExecutor::ClassGraph *cg;
	int errcode = QExecutor::ClassGraph::getHandle(cg);
	if (errcode) {ec->printf("InterfaceMaps::getClassBindForInterface: error - no handle\n"); return;}
	cg->getClassLidByName(cName, classGraphLid);
	if ((classGraphLid) && (!(cg->vertexExist(classGraphLid)))) classGraphLid = NULL;
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
		
	printAll();
	m_cache.clear();
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
	m_bindMap.removeBind(oN);
	m_cache.clear();
	
	printAll();
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


/****************************************
 *				BindMap					*
 ***************************************/

void BindMap::addBind(string interface, string objectName, string impl, string inv)
{
	m_objNameToInt[objectName] = interface;
	ImplementationInfo iI;
	iI.setName(impl);
	iI.setInvName(inv);
	m_intToImp[interface] = iI;
}

void BindMap::removeBind(const string& obj)
{
	if (m_objNameToInt.find(obj) == m_objNameToInt.end())
		return;
	string interface = m_objNameToInt[obj];
	m_objNameToInt.erase(obj);
	m_intToImp.erase(interface);
}

void BindMap::clear()
{
	m_objNameToInt.clear();
	m_intToImp.clear();
}

string BindMap::getIntForObj(const string& oName) const
{
	string out = "";
	TDict::const_iterator it = m_objNameToInt.find(oName);
	if (it != m_objNameToInt.end())
	{
		out = (*it).second;
	}
	return out;
}

string BindMap::getInvForInt(const string& iName) const
{
	string out = "";
	TIntToImpInfo::const_iterator it = m_intToImp.find(iName);
	if (it != m_intToImp.end())
	{
		out = (*it).second.getInvName();
	}
	return out;
}

string BindMap::getClassForInt(const string& iName) const
{
	string out = "";
	TIntToImpInfo::const_iterator it = m_intToImp.find(iName);
	if (it != m_intToImp.end())
	{
		out = (*it).second.getName();
	}
	return out;
}

void BindMap::print(ErrorConsole *ec) const
{
	for (TDict::const_iterator bit = m_objNameToInt.begin(); bit != m_objNameToInt.end(); ++bit)
	{
		string objectName = (*bit).first;
		string interface = (*bit).second;
		TIntToImpInfo::const_iterator iit = m_intToImp.find(interface);
		if (iit == m_intToImp.end())
		{
			ec->printf("BindMap::print() --> BindMap corrupted on %s entry\n", interface.c_str());
			return;
		}
		ImplementationInfo it = (*iit).second;
		string className = it.getName();
		string invName = it.getInvName();
		ec->printf("%s->%s, %s->(%s, %s)\n", objectName.c_str(), interface.c_str(), interface.c_str(), className.c_str(), invName.c_str());
	}
}

