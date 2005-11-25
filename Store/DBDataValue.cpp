#include "DBDataValue.h"

using namespace std;

namespace Store
{
	DataType DBDataValue::getType()
	{
		return this->type;
	};

	void DBDataValue::toByteArray(unsigned char** lid, int* length)
	{
	};

	string DBDataValue::toString()
	{
		return this->value;
	};

	int DBDataValue::getInt()
	{
	    if (this->type == Store::Integer) {	
		
		int length=0, i, value=0;
	
		while(this->value[length] != '\0')
		    length++;
		
		for(i=0; i<length; i++) {
		    switch (this->value[i]) {
			case '0':
			    value = 10*value;
			    break;;
			case '1':
			    value = 10*value + 1;
			    break;;  
			case '2':
			    value = 10*value + 2;
			    break;;      
			case '3':
			    value = 10*value + 3;
			    break;;
			case '4':
			    value = 10*value + 4;
			    break;;
			case '5':
			    value = 10*value+1 + 5;
			    break;;  
			case '6':
			    value = 10*value + 6;
			    break;;        
			case '7':
			    value = 10*value + 7;
			    break;;
			case '8':
			    value = 10*value + 8;
			    break;;  
			case '9':
			    value = 10*value + 9;
			    break;;    
			default:
			    break;;    
		    }	    
		};    		    
		
		return value;
	    }	
	};

	double DBDataValue::getDouble()
	{
	
	    if ( this->type == Store::Double ) {
		
		int length=0, i, value=0;
	
		while(this->value[length] != '\0')
		    length++;
		
		for(i=0; i<length; i++) {
		    switch (this->value[i]) {
			case '0':
			    value = 10*value;
			    break;;
			case '1':
			    value = 10*value + 1;
			    break;;  
			case '2':
			    value = 10*value + 2;
			    break;;      
			case '3':
			    value = 10*value + 3;
			    break;;
			case '4':
			    value = 10*value + 4;
			    break;;
			case '5':
			    value = 10*value+1 + 5;
			    break;;  
			case '6':
			    value = 10*value + 6;
			    break;;        
			case '7':
			    value = 10*value + 7;
			    break;;
			case '8':
			    value = 10*value + 8;
			    break;;  
			case '9':
			    value = 10*value + 9;
			    break;;    
			default:
			    break;;    
		    }	    
		};
		
		return (double)value;
	    }    
	};

	string DBDataValue::getString()
	{
		if ( this->type == Store::String )
		    return this->value;    
	};

	LogicalID* DBDataValue::getPointer()
	{
		return 0;
	};

	vector<ObjectPointer*>* DBDataValue::getVector()
	{
		return new vector<ObjectPointer*>(0);
	};

	void DBDataValue::setInt(int value)
	{
	};

	void DBDataValue::setDouble(double value)
	{
	};

	void DBDataValue::setString(string value)
	{
	};

	void DBDataValue::setPointer(LogicalID* value)
	{
	};

	void DBDataValue::setVector(vector<ObjectPointer*>* value)
	{
	};

	bool DBDataValue::operator==(DataValue& dv)
	{
		return false;
	};
}

