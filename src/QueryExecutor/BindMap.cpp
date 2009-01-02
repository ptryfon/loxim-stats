#include "BindMap.h"
#include "TransactionManager/Transaction.h"

using namespace Schemas;

BindMap::BindMap() 
{
	clear();
}

void BindMap::addToReverseMap(string interface, string implementation)
{
	set<string> s = m_reverseMap[implementation];
	s.insert(interface);
	m_reverseMap[implementation] = s;
}

void BindMap::removeInterfaceFromReverseMap(string interface, string implementation)
{
	set<string> s = m_reverseMap[implementation];
	s.erase(interface);
	m_reverseMap[implementation] = s;
}

void BindMap::addBind(string interface, string boundName, string boundObjectName, BindType t)
{
	ImplementationInfo iI(t);
	iI.setName(boundName);
	iI.setObjectName(boundObjectName);
	m_intToImp[interface] = iI;
	addToReverseMap(interface, boundName);
}

void BindMap::removeBind(const string& interface)
{
	ImplementationInfo iI = m_intToImp[interface];
	m_intToImp.erase(interface);
	removeInterfaceFromReverseMap(interface, iI.getName());
}

void BindMap::removeEntriesForImplementation(string implementationName)
{
	set<string> s = m_reverseMap[implementationName];
	for (set<string>::iterator it = s.begin(); it != s.end(); ++it)
	{
		string interfaceName = (*it);
		removeBind(interfaceName);
	}
	m_reverseMap.erase(implementationName);
}

set<string> BindMap::interfacesShowingImplementation(string implementationName)
{
	return m_reverseMap[implementationName];
}

void BindMap::changeObjectNameForImplementation(string implementationName, string newObjectName)
{
	set<string> s = m_reverseMap[implementationName];
	for (set<string>::iterator it = s.begin(); it != s.end(); ++it)
	{
		string interfaceName = (*it);
		ImplementationInfo iI = m_intToImp[interfaceName];
		iI.setObjectName(newObjectName);
		m_intToImp[interfaceName] = iI;
	}
}

void BindMap::clear()
{
	m_intToImp.clear();
	m_reverseMap.clear();
}

ImplementationInfo BindMap::getImpForInterface(const string& iName, bool &found, bool final) const
{
	ImplementationInfo out;
	TIntToImpInfo::const_iterator it = m_intToImp.find(iName);
	found = false;
	if (it != m_intToImp.end())
	{		
		found = true;
		out = (*it).second;
		if ((final) && (out.getType() == BIND_INTERFACE))
			out = getImpForInterface(out.getName(), found, final);
	}
	
	return out;
}

void BindMap::print(ErrorConsole *ec) const
{
	for (TIntToImpInfo::const_iterator bit = m_intToImp.begin(); bit != m_intToImp.end(); ++bit)
	{
		ImplementationInfo i = (*bit).second;
		string interface = (*bit).first;
		string impName = i.getName();
		string impObjName = i.getObjectName();
		debug_printf(*ec, "%s->(%s,%s)\n", interface.c_str(), impName.c_str(), impObjName.c_str());
	}
}

