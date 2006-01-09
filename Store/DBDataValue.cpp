#include "DBDataValue.h"

using namespace std;

namespace Store
{
	DBDataValue::DBDataValue()
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
	
	DBDataValue::DBDataValue(unsigned char *fbArray)
	{
		p_init();
		//type = odczytanyzpierwszegointa;
		//value. = ;
	};

	DBDataValue::~DBDataValue()
	{
		p_destroyVal();
	};
	 
	DataType DBDataValue::getType()
	{
		return type;
	};

	void DBDataValue::toByteArray(unsigned char** buff, int* length)
	{
	};

	void DBDataValue::toFullByteArray(unsigned char** buff, int* length)
	{
		//full czyli type + value
		int etyp = static_cast<int>(type);
		int size = sizeof(int);
		unsigned char *barray;
		vector<pair<unsigned char*,int> > vecbarray;
		vector<ObjectPointer*>::iterator lvec_iter;
		int basize;
		switch(type) {
			case Store::Integer: size+= sizeof(int); break;
			case Store::Double:  size+= sizeof(double); break;
			case Store::String:  size+= sizeof(int) +
				value.string_value->length(); break;
			case Store::Pointer:
				value.pointer_value->toByteArray(&barray, &basize);
				size+= basize; break;
			case Store::Vector:
				for(lvec_iter = value.vector_value->begin();
					lvec_iter != value.vector_value->end(); lvec_iter++)
				{
//					lvec_iter.toByteArray(&barray, &basize);
					vecbarray.push_back(make_pair(barray, basize));
				}
				break;
			default: break;
		};
		
		*buff = new unsigned char[size];
		*(reinterpret_cast<int*>(*buff)) = etyp;
		int wpos = sizeof(int);
		switch(type) {
			case Store::Integer:
				*(reinterpret_cast<int*>((*buff)+wpos)) = *(value.int_value);
				break;
			case Store::Double:
				*(reinterpret_cast<double*>((*buff)+wpos)) = *(value.double_value);
				break;
			case Store::String: {
				int l = value.string_value->length();
				for(int i=0; i<l; i++)
					*(reinterpret_cast<char*>((*buff)+wpos++)) =
						(*(value.string_value))[i];
			} break;
			case Store::Pointer:
				for(int i=0; i<basize; i++)
					(*buff)[wpos++] = barray[i];
				break;
			case Store::Vector:   break;
			default: break;
		};
		
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

	vector<ObjectPointer*>* DBDataValue::getVector()
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

	void DBDataValue::setVector(vector<ObjectPointer*>* val)
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
	
	
	void DBDataValue::p_init()
	{
		p_clearPtr();
		type = Store::Integer;
		value.int_value     = new int(0);
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
//				if(value.pointer_value) delete value.pointer_value;
				break;
			case Store::Vector:
//				if(value.vector_value) delete value.vector_value;
				break;
		}
		p_clearPtr();
	};
	
	void DBDataValue::p_clearPtr()
	{
		value.int_value = NULL;
		//could clear all fields bu this is an union (4bytes?)
	}
}

