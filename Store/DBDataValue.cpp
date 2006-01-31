#include "DBDataValue.h"
#include <stdio.h>

using namespace std;

namespace Store
{
	DBDataValue::DBDataValue() // konstruktory DV robia deep-copy
	{
		p_init();
	};
	
	DBDataValue::DBDataValue(int val)
	{
		p_init();
		type = Store::Integer;
		value.int_value = new int(val);
	};

	DBDataValue::DBDataValue(double val)
	{
		p_init();
		type = Store::Double;
		value.double_value = new double(val);
	};

	DBDataValue::DBDataValue(string val)
	{
		p_init();
		type = Store::String;
		value.string_value = new string(val);
	};

	DBDataValue::DBDataValue(LogicalID* val)
	{
		p_init();
		type = Store::Pointer;
		value.pointer_value = new DBLogicalID();
		*(value.pointer_value) = *val;
	};

	DBDataValue::DBDataValue(vector<LogicalID*>* val)
	{
		p_init();
		type = Store::Vector;
		value.vector_value = new vector<LogicalID*>;
		cout << "dd 1\n";
		vector<LogicalID*>::iterator oi;
		for(oi=val->begin(); oi!=val->end(); oi++){
				cout << "dd 2\n";
				cout << *((int*)(&oi)) << endl;
				if((*oi) == NULL) cout << "dd 2aa\n";
				cout << "gggg" << endl;
				int ilid= (*oi)->toInteger();
				cout << "dd 2bb\n";
			LogicalID* lid = new DBLogicalID(ilid);
			value.vector_value->push_back(lid);
					cout << "dd 3\n";
			}
	};

/*	DBDataValue::DBDataValue(DataValue& dv)
	{
		p_init();
		type = dv.getType();
		switch(type) {
			case Store::Integer:
				str << *(value.int_value); break;
			case Store::Double:
				str << *(value.double_value); break;
			case Store::String:
				str << *(value.string_value); break;
			default:
				str << "ptr_or_vect"; break;
		}
		value = ;
	};
*/
	DBDataValue::~DBDataValue()
	{
		p_destroyVal();
	};
	 
	DataType DBDataValue::getType()
	{
		return type;
	};

	string DBDataValue::toString()
	{
		ostringstream str;
		switch(type) {
			case Store::Integer:
				str << *(value.int_value); break;
			case Store::Double:
				str << *(value.double_value); break;
			case Store::String:
				str << *(value.string_value); break;
			default:
				str << "ptr_or_vect"; break;
		}
		return str.str();
	};

	Serialized DBDataValue::serialize() const
	{
		Serialized s;
		s += static_cast<int>(type);
cout << "d 1 " << type << endl;
		switch(type) {
			case Store::Integer: s += *value.int_value; break;
			case Store::Double:  s += *value.double_value; break;
			case Store::String:  s += *value.string_value; break;
			case Store::Pointer: s += *value.pointer_value; break;
			case Store::Vector: {
				vector<LogicalID*>::iterator obj_iter;
				cout << "d 2 \n";
				s += static_cast<int>(value.vector_value->size());
				cout << "d 3 \n";
				for(obj_iter = value.vector_value->begin();
						obj_iter != value.vector_value->end(); obj_iter++)
					{cout << "d 4 \n";
					s += *(*obj_iter);}
				} break;
			default:
				break;
		}
		return s;
	};

	int DBDataValue::deserialize(unsigned char* bytes, DBDataValue*& value)
	{
		unsigned char *curpos = bytes;
		DataType type = static_cast<DataType>(
			*(reinterpret_cast<int*>(curpos)) );
		curpos += sizeof(int);
		
		switch(type) {
			case Store::Integer:
				value = new DBDataValue(*(reinterpret_cast<int*>(curpos)));
				return ((curpos-bytes)+sizeof(int));
				break;
			case Store::Double:
				value = new DBDataValue(*(reinterpret_cast<double*>(curpos)));
				return ((curpos-bytes)+sizeof(double));
				break;
			case Store::String: {
				int slen = *(reinterpret_cast<int*>(curpos));
				curpos += sizeof(int);
				string s;
				for(int i=0; i<slen; i++)
					s += *(reinterpret_cast<char*>(curpos++));
				value = new DBDataValue(s);
				return (curpos-bytes);
			} break;
			case Store::Pointer: {
				DBLogicalID *lid;
				int ub = DBLogicalID::deserialize(curpos, lid);
				value = new DBDataValue();
				value->setPointer(lid);
				return ((curpos-bytes)+ub);
			} break;
			case Store::Vector: {
				int vlen = *(reinterpret_cast<int*>(curpos));
				curpos += sizeof(int);
				vector<LogicalID*>* v = new vector<LogicalID*>;
				DBLogicalID *lid;
				int ub;
				for(int i=0; i<vlen; i++) {
					ub = DBLogicalID::deserialize(curpos, lid);
					curpos += ub;
					v->push_back(lid);
				}
				value = new DBDataValue();
				value->setVector(v);
				return (curpos-bytes);
			} break;
			default:
				break;
		}		
		return -1;
	};

	int DBDataValue::getInt()
	{
		if( type == Store::Integer )
			return *(value.int_value);
		return 0;    	
	};

	double DBDataValue::getDouble()
	{
		if( type == Store::Double )
			return *(value.double_value);
		return 0;
	};

	string DBDataValue::getString()
	{
		if( type == Store::String )
			return *(value.string_value);
		return "";
	};

	LogicalID* DBDataValue::getPointer()
	{
		if( type == Store::Pointer )
			return value.pointer_value;
		return NULL;
	};

	vector<LogicalID*>* DBDataValue::getVector()
	{
		if( type == Store::Vector )
			return value.vector_value;
		return NULL;
	};

	void DBDataValue::setInt(int val)
	{
		p_destroyVal();
		type = Store::Integer;
		value.int_value = new int(val);
	};

	void DBDataValue::setDouble(double val)
	{
		p_destroyVal();
		type = Store::Double;
		value.double_value = new double(val);
	};

	void DBDataValue::setString(string val)
	{
		p_destroyVal();
		type = Store::String;
		value.string_value = new string(val);
	};

	void DBDataValue::setPointer(LogicalID* val)
	{
		p_destroyVal();
		type = Store::Pointer;
		value.pointer_value = val;
	};

	void DBDataValue::setVector(vector<LogicalID*>* val)
	{
		p_destroyVal();
		type = Store::Vector;
		value.vector_value = val;
	};

	bool DBDataValue::operator==(DataValue& dv)
	{
		if( this->getType() != dv.getType() )
			return false;
		switch( this->getType() ) {
			case Store::Integer:
				return (this->getInt() == dv.getInt());
			case Store::Double:
				return (this->getDouble() == dv.getDouble());
			case Store::String:
				return (this->getString() == dv.getString());
			case Store::Pointer:
				return (this->getPointer() == dv.getPointer());
			case Store::Vector:
				return (this->getVector() == dv.getVector());
		}
		return false;
	};
	
	DataValue& DBDataValue::operator=(const int& val)
	{
		p_destroyVal();
		type = Store::Integer;
		value.int_value = new int(val);
		return *this;
	};

	DataValue& DBDataValue::operator=(const double& val)
	{
		p_destroyVal();
		type = Store::Double;
		value.double_value = new double(val);
		return *this;
	};

	DataValue& DBDataValue::operator=(const string& val)
	{
		p_destroyVal();
		type = Store::String;
		value.string_value = new string(val);
		return *this;
	};

	DataValue& DBDataValue::operator=(const LogicalID& val)
	{
		p_destroyVal();
		type = Store::Pointer;
		value.pointer_value = val.clone();
		return *this;
	};

	DataValue& DBDataValue::operator=(const vector<LogicalID*>& val)
	{
		p_destroyVal();
		type = Store::Vector;
		value.vector_value = new vector<LogicalID*>(0);
		vector<LogicalID*>::iterator oi;
		vector<LogicalID*>* pval = const_cast<vector<LogicalID*>*>(&val);
		for(oi=pval->begin();oi!=pval->end(); oi++);
			value.vector_value->push_back((*oi)->clone());
		return *this;
	};

	void DBDataValue::p_init()
	{
		p_clearPtr();
		type = Store::Integer;
		value.int_value = new int(0);
	};
	
	void DBDataValue::p_destroyVal()
	{
		switch(type) {
			case Store::Integer:
				if(value.int_value) delete value.int_value; break;
			case Store::Double:
				if(value.double_value) delete value.double_value; break;
			case Store::String:
				if(value.string_value) delete value.string_value; break;
			case Store::Pointer:
				if(value.pointer_value) delete value.pointer_value;
				break;
			case Store::Vector:
				if(value.vector_value) {
					vector<LogicalID*>::iterator obj_iter;
					for(obj_iter = value.vector_value->begin();
							obj_iter != value.vector_value->end(); obj_iter++)
						delete (*obj_iter);
				} delete value.vector_value;
				break;
		}
		p_clearPtr();
	};
	
	void DBDataValue::p_clearPtr()
	{
		value.int_value = NULL;
		//could clear all fields but this is an union (4bytes?)
	}
}
