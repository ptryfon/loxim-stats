package pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper;

public class NumericCppTypeHelper extends SimpleCppTypeHelper {
	
	@Override
	public String getTypeDeclaration() {
		return getTypeName()+"_t";
	}
	
}
