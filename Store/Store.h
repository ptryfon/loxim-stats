#ifndef __STORE_H__
#define __STORE_H__

namespace Store
{
//	class PhysicalID;
	class LogicalID;
	class DataValue;
	class ObjectPointer;
	class StoreManager;

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
	};

	enum AccessMode
	{
		Read	= 0x01,
		Write	= 0x02,
	};
};

#include <string>
#include <vector>
#include "Config/SBQLConfig.h"
#include "Log/Logs.h"
#include "TransactionManager/Transaction.h"
#include "DBPhysicalID.h"

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
		virtual DBPhysicalID* getPhysicalID() = 0;
		virtual void toByteArray(unsigned char** lid, int* length) = 0;
		virtual string toString() const = 0;
		virtual unsigned int toInteger() const = 0;
		virtual Serialized serialize() const = 0;
		virtual LogicalID* clone() const = 0;

		// Operators
		virtual bool operator==(LogicalID& lid) = 0;
		virtual ~LogicalID() {};
	};


	class DataValue
	{
	public:
		// Class functions
		virtual DataType getType() const = 0;
		virtual ExtendedType getSubtype() const = 0;
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
		virtual int addRoot(TransactionID* tid, ObjectPointer*& object) = 0;
		virtual int removeRoot(TransactionID* tid, ObjectPointer*& object) = 0;

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
		virtual int dataValueFromByteArray(unsigned char* buffer, DataValue*& value) = 0;
		
		// Misc
		virtual LogManager* getLogManager() = 0;
		virtual DBPhysicalID* getPhysicalID(LogicalID* lid) = 0;
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
		Serialized& operator+=(const double&);
		Serialized& operator+=(const string&);
		Serialized& operator+=(const LogicalID&);
		Serialized& operator+=(const DataValue&);
		template <typename T> Serialized& operator+=(const T&);
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
