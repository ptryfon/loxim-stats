package pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper;

import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public abstract class AbstractStringCppTypeHelper extends AbstractCppTypeHelper {
	//private String typeName;
	

	private Integer minSize;
	private Integer maxSize;
	
	
	@Override
	public String getTypeDeclaration() {
		return "CharArray*";
	}
	
	@Override
	public String getInitValue() {
		return "NULL";
	}
	
	public String destroyCode(String name)
	{
		return "if ("+name+") delete "+name+";";
	}
	
/*	public String equalsCode(String name)
	{
		return "if (!CharArray::equal(get"+CodeHelper.upperFirstChar(name)+"(),p->get"+CodeHelper.upperFirstChar(name)+"()) return false;";
	}
*/	
	public String compareCode(String var1,String var2)
	{
		return "(CharArray::equal("+var1+","+var2+"))";
	}
	
	public boolean isMappable() {
		return false;
	}
	
	public String getMapType() {
		return null;
	}
	
	public String isInCode(String value, String string) {
		return null;
	}
	
	public void fieldInitialize(FieldType field, ProtocolTypeWrapper protocol) {
		//super.fieldInitialize(field);
		if(field.getSize()!=null)
			setMaxSize(field.getSize().intValue());
	}	
	

	public Integer getMaxSize() {
		return maxSize;
	}
	public Integer getMinSize() {
		return minSize;
	}
	public void setMaxSize(Integer maxSize) {
		this.maxSize = maxSize;
	}
	public void setMinSize(Integer minSize) {
		this.minSize = minSize;
	}
	
	public String getIncludeCode() {
		return null;
	}

}
