#ifndef __STORE_H__
#define __STORE_H__

//#define DEBUG_MODE

#include <set>
#include <string>
#include <vector>

#include <QueryExecutor/HashMacro.h>
#include <Config/SBQLConfig.h>
#include <Store/DBPhysicalID.h>

namespace TManager {
	class Transaction;
	class TransactionID;
}

namespace Logs {
	class LogManager;
}


namespace Store
{
//	class PhysicalID;
	class LogicalID;
	class DataValue;
	class ObjectPointer;
	class StoreManager;
	class NamedRoots;
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

	class DataValue;
	class LogicalID;

	class Serialized
	{
		public:
		Serialized();
		Serialized(int size);
		Serialized(const Serialized&);
		Serialized& operator=(const Serialized&);
		virtual ~Serialized();
		Serialized& operator+=(const Serialized&);
		Serialized& operator+=(const int&);
		Serialized& operator+=(const unsigned int&);
		Serialized& operator+=(const double&);
		Serialized& operator+=(const std::string&);
		Serialized& operator+=(const LogicalID&);
		Serialized& operator+=(const DataValue&);
		Serialized& operator+=(SetOfLids* const &s);
		//Serialized& operator+=(const T&);
		void info() const;

		unsigned char* bytes;
		int size;
	private:
		int realsize;
		template <typename T> Serialized p_baseTypeSerialize(const T&);
	};


	class LogicalID
	{
	public:
		// Class functions
		virtual DBPhysicalID* getPhysicalID(TManager::TransactionID* tid) = 0;
		virtual void toByteArray(unsigned char** lid, int* length) = 0;
		virtual std::string toString() const = 0;
		virtual unsigned int toInteger() const = 0;
		virtual Serialized serialize() const = 0;
		virtual LogicalID* clone() const = 0;
		virtual std::string getServer() const = 0;
		virtual void setServer(std::string server) = 0;
		virtual int getPort() const = 0;
		virtual void setPort(int port) = 0;
		virtual LogicalID* getRemoteID()  const = 0;
		virtual void setRemoteID(LogicalID* remoteID) = 0;
		virtual std::string getParentRoot() const = 0;
		virtual void setParentRoot(std::string parentRoot) = 0;
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
		virtual std::string toString() = 0;
		virtual Serialized serialize() const = 0;
		virtual DataValue* clone() const = 0;

		// Specific type accessors
		virtual int getInt() const = 0;
		virtual double getDouble() const = 0;
		virtual std::string getString() const = 0;
		virtual LogicalID* getPointer() const = 0;
		virtual vector<LogicalID*>* getVector() const = 0;

		virtual void setInt(int value) = 0;
		virtual void setDouble(double value) = 0;
		virtual void setString(std::string value) = 0;
		virtual void setPointer(LogicalID* value) = 0;
		virtual void setVector(vector<LogicalID*>* value) = 0;

		// Operators
		virtual bool operator==(DataValue& dv) = 0;
		virtual DataValue& operator=(const int& val) = 0;
		virtual DataValue& operator=(const double& val) = 0;
		virtual DataValue& operator=(const std::string& val) = 0;
		virtual DataValue& operator=(const LogicalID& val) = 0;
		virtual DataValue& operator=(const vector<LogicalID*>& val) = 0;
		virtual ~DataValue() {};
	};


	class ObjectPointer
	{
	public:
		// Class functions
		virtual LogicalID* getLogicalID() const = 0;
		virtual std::string getName() const = 0;
		virtual AccessMode getMode() const = 0;
		virtual DataValue* getValue() const = 0;
		virtual void setValue(DataValue* val) = 0;
		virtual std::string toString() = 0;
		virtual Serialized serialize() const = 0;
		virtual bool getIsRoot() const = 0;
		virtual void setIsRoot(bool isRoot) = 0;
		virtual vector<LogicalID*>* getClasses() const = 0;
		virtual void setClasses(vector<LogicalID*>* value) = 0;
		virtual std::string getParentRoot() const = 0;
		virtual void setParentRoot(std::string parentRoot) = 0;

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
		virtual int getObject(TManager::TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object) = 0;
		virtual int createObject(TManager::TransactionID* tid, std::string name, DataValue* value, ObjectPointer*& object, LogicalID* lid=NULL) = 0;
		virtual int deleteObject(TManager::TransactionID* tid, ObjectPointer* object) = 0;
		virtual int modifyObject(TManager::TransactionID* tid, ObjectPointer*& object, DataValue* value) = 0;

		virtual int replaceDV(ObjectPointer* obj, DataValue* dv) = 0;

		// Roots
		virtual int getRoots(TManager::TransactionID* tid, vector<ObjectPointer*>*& roots) = 0;
		virtual int getRoots(TManager::TransactionID* tid, std::string name, vector<ObjectPointer*>*& roots) = 0;
		virtual int getRootsLID(TManager::TransactionID* tid, vector<LogicalID*>*& roots) = 0;
		virtual int getRootsLID(TManager::TransactionID* tid, std::string name, vector<LogicalID*>*& roots) = 0;
		virtual int getRootsLIDWithBegin(TManager::TransactionID* tid, std::string nameBegin, vector<LogicalID*>*& roots) = 0;
		virtual int addRoot(TManager::TransactionID* tid, ObjectPointer*& object) = 0;
		virtual int removeRoot(TManager::TransactionID* tid, ObjectPointer*& object) = 0;
		virtual NamedRoots* getRoots() = 0;


		// Views
		virtual int getViewsLID(TManager::TransactionID* tid, vector<LogicalID*>*& roots) = 0;
		virtual int getViewsLID(TManager::TransactionID* tid, std::string name, vector<LogicalID*>*& roots) = 0;
		virtual int addView(TManager::TransactionID* tid, const char* name, ObjectPointer*& object) = 0;
		virtual int removeView(TManager::TransactionID* tid, ObjectPointer*& object) = 0;

		// Classes
		virtual int getClassesLID(TManager::TransactionID* tid, vector<LogicalID*>*& roots) = 0;
		virtual int getClassesLID(TManager::TransactionID* tid, std::string name, vector<LogicalID*>*& roots) = 0;
		virtual int getClassesLIDByInvariant(TManager::TransactionID* tid, std::string invariantName, vector<LogicalID*>*& roots) = 0;
		virtual int addClass(TManager::TransactionID* tid, const char* name, const char* invariantName, ObjectPointer*& object) = 0;
		virtual int removeClass(TManager::TransactionID* tid, ObjectPointer*& object) = 0;

		// Interfaces
		virtual int getInterfacesLID(TManager::TransactionID* tid, vector<LogicalID*>*& roots) = 0;
		virtual int getInterfacesLID(TManager::TransactionID* tid, std::string name, vector<LogicalID*>*& roots) = 0;
		virtual int addInterface(TManager::TransactionID* tid, const std::string& name, const std::string& objectName, ObjectPointer*& object) = 0;
		virtual int bindInterface(TManager::TransactionID* tid, const std::string& name, const std::string& bindName) = 0;
		virtual int getInterfaceBindForObjectName(TManager::TransactionID *tid, const std::string& oName, std::string& iName, std::string& bName) = 0;
		virtual int removeInterface(TManager::TransactionID* tid, ObjectPointer*& object) = 0;

		// Schemas
		virtual int getSchemasLID(TManager::TransactionID* tid, vector<LogicalID*>*& s) = 0;
		virtual int getSchemasLID(TManager::TransactionID* tid, std::string name, vector<LogicalID*> *&s) = 0;
		virtual int addSchema(TManager::TransactionID* tid, std::string name, ObjectPointer *&object) = 0;
		virtual int removeSchema(TManager::TransactionID* tid, ObjectPointer *&object) = 0;
 		
		// System views
		virtual int getSystemViewsLID(TManager::Transaction* tr, vector<LogicalID*>*& p_systemviews) = 0;
		virtual int getSystemViewsLID(TManager::Transaction* tr, std::string name, vector<LogicalID*>*& p_systemviews) = 0;

		// Transactions
		virtual int abortTransaction(TManager::TransactionID* tid) = 0;
		virtual int commitTransaction(TManager::TransactionID* tid) = 0;

		// Data creation
		virtual DataValue* createIntValue(int value) = 0;
		virtual DataValue* createDoubleValue(double value) = 0;
		virtual DataValue* createStringValue(std::string value) = 0;
		virtual DataValue* createPointerValue(LogicalID* value) = 0;
		virtual DataValue* createVectorValue(vector<LogicalID*>* value) = 0;

		virtual LogicalID* createLID(unsigned int) = 0;

		// Object Creation for Logs
		virtual ObjectPointer* createObjectPointer(LogicalID* lid) = 0;
		virtual ObjectPointer* createObjectPointer(LogicalID* lid, std::string name, DataValue* dv) = 0;

		// Deserialization
		virtual int logicalIDFromByteArray(unsigned char* buffer, LogicalID*& lid) = 0;
		virtual int dataValueFromByteArray(TManager::TransactionID* id, unsigned char* buffer, DataValue*& value) = 0;

		// Misc
		virtual Logs::LogManager* getLogManager() = 0;
		virtual DBPhysicalID* getPhysicalID(TManager::TransactionID* tid, LogicalID* lid) = 0;
		virtual int checkpoint(unsigned int& cid) = 0;
		virtual int endCheckpoint(unsigned int& cid) = 0;

		// Operators
		virtual ~StoreManager() {};
	};

};

#endif
