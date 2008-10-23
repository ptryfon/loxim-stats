package pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper;

import java.util.List;

import pl.edu.mimuw.loxim.protogen.api.values.UniversalValuesSource;


public class StringCppTypeHelper extends AbstractStringCppTypeHelper {

	public String readAssignCode(String name)
	{
		return name+"=reader->readString();";
	}
	
	public String writeAssignCode(String name)
	{
		return "writer->writeString("+name+");";
	}
	
	public String getTypeName() 
	{
		return "string";
	}

	public List<String> getValues() {
		return UniversalValuesSource.getInstance().getStringValues();
	}
	
}
