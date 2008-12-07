#ifndef _BINDMAP_H_
#define _BINDMAP_H_
#include <map>
#include <vector>
#include <string>

namespace Store
{
	class LogicalID;
}

namespace Errors
{
	class ErrorConsole;
}

using namespace std;
using namespace Store;
using namespace Errors;

namespace Schemas
{
	typedef vector<LogicalID*> TLidsVector;

	enum BindType
	{
		BIND_CLASS,
		BIND_VIEW,
		BIND_INTERFACE	
	};

	class ImplementationInfo
	{	
		private:
			string m_name;
			string m_objectName;
			BindType m_type;
		
		public:
			ImplementationInfo(BindType t = BIND_CLASS) {m_type = t;}
			void setName(string n) {m_name = n;}
			void setObjectName(string n) {m_objectName = n;}
			string getName() const {return m_name;}
			string getObjectName() const {return m_objectName;}
			BindType getType() const {return m_type;}
	};

	typedef map<string, string> TDict;
	typedef map<string, ImplementationInfo> TIntToImpInfo;

	class BindMap
	{
		private:
			TIntToImpInfo m_intToImp;

		public:
			BindMap();
			void addBind(string interface, string boundName, string boundObjectName, BindType type);
			void removeBind(const string& interface);
			bool hasBind(const string& interface) const {return (m_intToImp.find(interface) != m_intToImp.end());}
			void clear();

			ImplementationInfo getImpForInterface(const string& iName, bool &found, bool final = false) const;

			void print(ErrorConsole *ec) const;
	};
	
}

#endif //_BINDMAP_H_
