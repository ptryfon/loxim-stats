package pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper;

import java.util.List;

import pl.edu.mimuw.loxim.protogen.api.values.UniversalValuesSource;


public class SStringCppTypeHelper extends AbstractStringCppTypeHelper {

	public String readAssignCode(String name)
	{
		return name+"=reader->readSstring();";
	}
	
	public String writeAssignCode(String name)
	{
		return "writer->writeSstring("+name+");";	
	}
	public String getTypeName() {
		return "sstring";
	}
	
	public List<String> getValues() {
		return UniversalValuesSource.getInstance().getSStringValues();
	}
}
