#ifndef __STORE_H__
#define __STORE_H__

//#define DEBUG_MODE

#include <set>
#include <QueryExecutor/HashMacro.h>




namespace Store
{
//	class PhysicalID;
	class LogicalID;
	class DataValue;
	class ObjectPointer;
	class StoreManager;
	struct LIDComparator;
	class hashLogicalID;
	struct eqLogicalID;
	typedef hash_set<LogicalID*, hashLogicalID, eqLogicalID> SetOfLids;
	//typedef std::set<LogicalID*, LIDComparator> SetOfLids;

	enum DataType
	{
		// Simple types
		Integer		= 0x01,
		Double		= 0x02,
		String		= 0x03,

		// Complex types
		Pointer		= 0xE0,
		Vector		= 0xF0,
	};

	// TODO
	enum ExtendedType
	{
		None = 0,
		Link = 1,		// String?
		Class = 2,		// Vector
		Procedure = 3,	// Vector
		View = 4,		// Vector
		Interface = 5,
		StoreSchema = 6
	};

	enum AccessMode
	{
		Read	= 0x01,
		Write	= 0x02,
	};
};

#include <string>
#include <vector>

#include <Config/SBQLConfig.h>
#include <Log/Logs.h>
#include <TransactionManager/Transaction.h>
#include <Store/DBPhysicalID.h>

/////////////////////////// WORDAROUND ///////////////////////////
//namespace Logs
//{
//	class LogManager {};
//};
//namespace TManager
//{
//	class TransactionID {};
//};
//namespace Config
//{
//	class SBQLConfig {};
//};
/////////////////////////// WORDAROUND ///////////////////////////

using namespace std;
using namespace Logs;
using namespace TManager;

namespace Store
{
	typedef struct Serialized Serialized;

// Using "PhysicalID" interface removed
//	include DBPhysicalID.h instead

/*	class PhysicalID
	{
	public:
		// Class functions
		virtual unsigned short getFile() = 0;
		virtual unsigned int getPage() = 0;
		virtual unsigned short getOffset() = 0;
		virtual long long getLockAddress() = 0;

		// Operators
		virtual ~PhysicalID() {};
	};
*/

	class LogicalID
	{
	public:
		// Class functions
		virtual DBPhysicalID* getPhysicalID(TransactionID* tid) = 0;
		virtual void toByteArray(unsigned char** lid, int* length) = 0;
		virtual string toString() const = 0;
		virtual unsigned int toInteger() const = 0;
		virtual Serialized serialize() const = 0;
		virtual LogicalID* clone() const = 0;
		virtual string getServer() const = 0;
		virtual void setServer(string server) = 0;
		virtual int getPort() const = 0;
		virtual void setPort(int port) = 0;
		virtual LogicalID* getRemoteID()  const = 0;
		virtual void setRemoteID(LogicalID* remoteID) = 0;
		virtual string getParentRoot() const = 0;
		virtual void setParentRoot(string parentRoot) = 0;
		virtual void setDirectParent (LogicalID *lid) = 0; //MH TC
		virtual LogicalID* getDirectParent() const = 0;	//MH TC
		// Operators
		virtual bool operator==(LogicalID& lid) = 0;
		virtual ~LogicalID() {};
	};

	struct LIDComparator {
		bool operator()(LogicalID* lid1, LogicalID* lid2) const {
			return lid1->toInteger() < lid2->toInteger();
		}
	};

	class hashLogicalID {
	private:
		hash<unsigned> hasher;

	public:
		size_t operator()(const LogicalID* lid) const {
			return hasher(lid->toInteger());
		}
	};

	struct eqLogicalID{
		bool operator()(const LogicalID* s1, const LogicalID* s2) const{
			return s1->toInteger() == s2->toInteger();
		}
	};


	class DataValue
	{
	public:
		// Class functions
		virtual DataType getType() const = 0;
		virtual ExtendedType getSubtype() const = 0;

		// ClassMarks methods
		virtual SetOfLids* getClassMarks() const = 0;
		virtual void addClassMark(LogicalID* classMark) = 0;
		virtual void setClassMarks(SetOfLids* classMarks) = 0;
		virtual void addClassMarks(SetOfLids* toAdd) = 0;
		virtual void removeClassMarks(SetOfLids* toDel) = 0;
		virtual void removeClassMark(LogicalID* lid) = 0;

		// Subclass methods
		virtual SetOfLids* getSubclasses() const = 0;
		virtual void addSubclass(LogicalID* subclass) = 0;
		virtual void setSubclasses(SetOfLids* subclasses) = 0;
		virtual void addSubclasses(SetOfLids* toAdd) = 0;
		virtual void removeSubclass(LogicalID* lid) = 0;

		virtual void setSubtype(ExtendedType type) = 0;
		virtual string toString() = 0;
		virtual Serialized serialize() const = 0;
		virtual DataValue* clone() const = 0;

		// Specific type accessors
		virtual int getInt() const = 0;
		virtual double getDouble() const = 0;
		virtual string getString() const = 0;
		virtual LogicalID* getPointer() const = 0;
		virtual vector<LogicalID*>* getVector() const = 0;

		virtual void setInt(int value) = 0;
		virtual void setDouble(double value) = 0;
		virtual void setString(string value) = 0;
		virtual void setPointer(LogicalID* value) = 0;
		virtual void setVector(vector<LogicalID*>* value) = 0;

		// Operators
		virtual bool operator==(DataValue& dv) = 0;
		virtual DataValue& operator=(const int& val) = 0;
		virtual DataValue& operator=(const double& val) = 0;
		virtual DataValue& operator=(const string& val) = 0;
		virtual DataValue& operator=(const LogicalID& val) = 0;
		virtual DataValue& operator=(const vector<LogicalID*>& val) = 0;
		virtual ~DataValue() {};
	};


	class ObjectPointer
	{
	public:
		// Class functions
		virtual LogicalID* getLogicalID() const = 0;
		virtual string getName() const = 0;
		virtual AccessMode getMode() const = 0;
		virtual DataValue* getValue() const = 0;
		virtual void setValue(DataValue* val) = 0;
		virtual string toString() = 0;
		virtual Serialized serialize() const = 0;
		virtual bool getIsRoot() const = 0;
		virtual void setIsRoot(bool isRoot) = 0;
		virtual vector<LogicalID*>* getClasses() const = 0;
		virtual void setClasses(vector<LogicalID*>* value) = 0;
		virtual string getParentRoot() const = 0;
		virtual void setParentRoot(string parentRoot) = 0;

		// Operators
		virtual bool operator==(ObjectPointer& dv) {
			return (this->getLogicalID() == dv.getLogicalID()); }
		virtual bool isRoot() { return true; }
		virtual ~ObjectPointer() {};
	};

	class StoreManager
	{
	public:
		static StoreManager* theStore;

		// Object
		virtual int getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object) = 0;
		virtual int createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object, LogicalID* lid=NULL) = 0;
		virtual int deleteObject(TransactionID* tid, ObjectPointer* object) = 0;
		virtual int modifyObject(TransactionID* tid, ObjectPointer*& object, DataValue* value) = 0;

		virtual int replaceDV(ObjectPointer* obj, DataValue* dv) = 0;

		// Roots
		virtual int getRoots(TransactionID* tid, vector<ObjectPointer*>*& roots) = 0;
		virtual int getRoots(TransactionID* tid, string name, vector<ObjectPointer*>*& roots) = 0;
		virtual int getRootsLID(TransactionID* tid, vector<LogicalID*>*& roots) = 0;
		virtual int getRootsLID(TransactionID* tid, string name, vector<LogicalID*>*& roots) = 0;
		virtual int getRootsLIDWithBegin(TransactionID* tid, string nameBegin, vector<LogicalID*>*& roots) = 0;
		virtual int addRoot(TransactionID* tid, ObjectPointer*& object) = 0;
		virtual int removeRoot(TransactionID* tid, ObjectPointer*& object) = 0;

		// Views
		virtual int getViewsLID(TransactionID* tid, vector<LogicalID*>*& roots) = 0;
		virtual int getViewsLID(TransactionID* tid, string name, vector<LogicalID*>*& roots) = 0;
		virtual int addView(TransactionID* tid, const char* name, ObjectPointer*& object) = 0;
		virtual int removeView(TransactionID* tid, ObjectPointer*& object) = 0;

		// Classes
		virtual int getClassesLID(TransactionID* tid, vector<LogicalID*>*& roots) = 0;
		virtual int getClassesLID(TransactionID* tid, string name, vector<LogicalID*>*& roots) = 0;
		virtual int getClassesLIDByInvariant(TransactionID* tid, string invariantName, vector<LogicalID*>*& roots) = 0;
		virtual int addClass(TransactionID* tid, const char* name, const char* invariantName, ObjectPointer*& object) = 0;
		virtual int removeClass(TransactionID* tid, ObjectPointer*& object) = 0;

		// Interfaces
		virtual int getInterfacesLID(TransactionID* tid, vector<LogicalID*>*& roots) = 0;
		virtual int getInterfacesLID(TransactionID* tid, string name, vector<LogicalID*>*& roots) = 0;
		virtual int addInterface(TransactionID* tid, const string& name, const string& objectName, ObjectPointer*& object) = 0;
		virtual int bindInterface(TransactionID* tid, const string& name, const string& bindName) = 0;
		virtual int getInterfaceBindForObjectName(TransactionID *tid, const string& oName, string& iName, string& bName) = 0;
		virtual int removeInterface(TransactionID* tid, ObjectPointer*& object) = 0;

		// Schemas
		virtual int getSchemasLID(TransactionID* tid, vector<LogicalID*>*& s) = 0;
		virtual int getSchemasLID(TransactionID* tid, string name, vector<LogicalID*> *&s) = 0;
		virtual int addSchema(TransactionID* tid, string name, ObjectPointer *&object) = 0;
		virtual int removeSchema(TransactionID* tid, ObjectPointer *&object) = 0;
 		
		// System views
		virtual int getSystemViewsLID(TransactionID* tid, vector<LogicalID*>*& p_systemviews) = 0;
		virtual int getSystemViewsLID(TransactionID* tid, string name, vector<LogicalID*>*& p_systemviews) = 0;

		// Transactions
		virtual int abortTransaction(TransactionID* tid) = 0;
		virtual int commitTransaction(TransactionID* tid) = 0;

		// Data creation
		virtual DataValue* createIntValue(int value) = 0;
		virtual DataValue* createDoubleValue(double value) = 0;
		virtual DataValue* createStringValue(string value) = 0;
		virtual DataValue* createPointerValue(LogicalID* value) = 0;
		virtual DataValue* createVectorValue(vector<LogicalID*>* value) = 0;

		// Object Creation for Logs
		virtual ObjectPointer* createObjectPointer(LogicalID* lid) = 0;
		virtual ObjectPointer* createObjectPointer(LogicalID* lid, string name, DataValue* dv) = 0;

		// Deserialization
		virtual int logicalIDFromByteArray(unsigned char* buffer, LogicalID*& lid) = 0;
		virtual int dataValueFromByteArray(TransactionID* id, unsigned char* buffer, DataValue*& value) = 0;

		// Misc
		virtual LogManager* getLogManager() = 0;
		virtual DBPhysicalID* getPhysicalID(TransactionID* tid, LogicalID* lid) = 0;
		virtual int checkpoint(unsigned int& cid) = 0;
		virtual int endCheckpoint(unsigned int& cid) = 0;

		// Operators
		virtual ~StoreManager() {};
	};

	typedef struct Serialized
	{
		Serialized();
		Serialized(int size);
		Serialized(const Serialized&);
		Serialized& operator=(const Serialized&);
		virtual ~Serialized();
		Serialized& operator+=(const Serialized&);
		Serialized& operator+=(const int&);
		Serialized& operator+=(const unsigned int&);
		Serialized& operator+=(const double&);
		Serialized& operator+=(const string&);
		Serialized& operator+=(const LogicalID&);
		Serialized& operator+=(const DataValue&);
		Serialized& operator+=(SetOfLids* const &s);
		template <class T> Serialized& operator+=(const T&);
		void info() const;

		unsigned char* bytes;
		int size;
	private:
		int realsize;
		template <typename T> Serialized p_baseTypeSerialize(const T&);
	} Serialized;

	//class Misc
	//{
	//public:
	//	Misc(){lastlid = 0;};
	//   vector <ObjectPointer*> vect;
	//	vector <ObjectPointer*> roots;
	//	int lastlid;
	//};

};

#endif