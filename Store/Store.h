#ifndef __STORE_H__
#define __STORE_H__

namespace Store
{
	class PhysicalID;
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
	class PhysicalID
	{
	public:
		// Class functions
		virtual unsigned short getFile() = 0;
		virtual unsigned int getPage() = 0;
		virtual unsigned short getOffset() = 0;

		// Operators
		virtual ~PhysicalID() {};
	};

	class LogicalID
	{
	public:
		// Class functions
		virtual PhysicalID* getPhysicalID() = 0;
		virtual void toByteArray(unsigned char** lid, int* length) = 0;
		virtual string toString() = 0;

		// Operators
		virtual bool operator==(LogicalID& lid) = 0;
		virtual ~LogicalID() {};
	};


	class DataValue
	{
	public:
		// Class functions
		virtual DataType getType() = 0;
		virtual void toByteArray(unsigned char** lid, int* length) = 0;
		virtual string toString() = 0;

		// Specific type accessors
		virtual int getInt() = 0;
		virtual double getDouble() = 0;
		virtual string getString() = 0;
		virtual LogicalID* getPointer() = 0;
		virtual vector<ObjectPointer*>* getVector() = 0;				// Good or bad idea??

		virtual void setInt(int value) = 0;
		virtual void setDouble(double value) = 0;
		virtual void setString(string value) = 0;
		virtual void setPointer(LogicalID* value) = 0;
		virtual void setVector(vector<ObjectPointer*>* value) = 0;		// Good or bad idea??

		// Operators
		virtual bool operator==(DataValue& dv) = 0;
		virtual ~DataValue() {};
	};


	class ObjectPointer
	{
	public:
		// Class functions
		virtual LogicalID* getLogicalID() = 0;
		virtual string getName() = 0;
		virtual AccessMode getMode() = 0;
		virtual DataValue* getValue() = 0;
		virtual void setValue(DataValue* val) = 0;
		virtual string toString() = 0;

		// Operators
		virtual bool operator==(ObjectPointer& dv) = 0;
		virtual ~ObjectPointer() {};
	};

	class StoreManager
	{
	public:
		// Object
		virtual int getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer** object) = 0;
		virtual int createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer** object) = 0;
		virtual int deleteObject(TransactionID* tid, ObjectPointer* object) = 0;

		// Roots
		virtual int getRoots(TransactionID* tid, vector<ObjectPointer*>** roots) = 0;
		virtual int getRoots(TransactionID* tid, string name, vector<ObjectPointer*>** roots) = 0;
		virtual int addRoot(TransactionID* tid, ObjectPointer* object) = 0;
		virtual int removeRoot(TransactionID* tid, ObjectPointer* object) = 0;

		// Transactions
		virtual int abortTransaction(TransactionID* tid) = 0;
		virtual int commitTransaction(TransactionID* tid) = 0;

		// Data creation
		virtual DataValue* createIntValue(int value) = 0;
		virtual DataValue* createDoubleValue(double value) = 0;
		virtual DataValue* createStringValue(string value) = 0;
		virtual DataValue* createVectorValue(vector<ObjectPointer*>* value) = 0;
		virtual DataValue* createPointerValue(ObjectPointer* value) = 0;

		// Deserialization
		virtual int logicalIDFromByteArray(unsigned char* buffer, int length, LogicalID** lid) = 0;
		virtual int dataValueFromByteArray(unsigned char* buffer, int length, DataValue** value) = 0;

		// Operators
		virtual ~StoreManager() {};
	};
};

#endif



