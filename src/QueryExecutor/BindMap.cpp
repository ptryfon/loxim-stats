#include "BindMap.h"
#include "TransactionManager/Transaction.h"

using namespace Schemas;

BindMap::BindMap() 
{
	clear();
}

void BindMap::addBind(string interface, string boundName, string boundObjectName, BindType t)
{
	ImplementationInfo iI(t);
	iI.setName(boundName);
	iI.setObjectName(boundObjectName);
	m_intToImp[interface] = iI;
}

void BindMap::removeBind(const string& interface)
{
	m_intToImp.erase(interface);
}

void BindMap::clear()
{
	m_intToImp.clear();
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
		ec->printf("%s->(%s,%s)\n", interface.c_str(), impName.c_str(), impObjName.c_str());
	}
}

