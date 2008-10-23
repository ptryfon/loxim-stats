package pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper;

public class BoolCppTypeHelper extends SimpleCppTypeHelper {
	
	@Override
	public String getTypeDeclaration() {
		return "bool";
	}
	
	@Override
	public String getTypeName() {
		return "bool";
	}
	
}
