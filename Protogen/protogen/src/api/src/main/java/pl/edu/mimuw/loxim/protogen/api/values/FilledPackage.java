package pl.edu.mimuw.loxim.protogen.api.values;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.Type;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageTypeWrapper;

public class FilledPackage {
	private PackageTypeWrapper _package;
	private Map<String, Object> field2value;
	
//	private static int allocated=0;

	public FilledPackage(PackageTypeWrapper a_package) {
		_package=a_package;
//		System.out.println("allocated: "+(++allocated));
	}
	
	public Map<String, Object> getField2value() {
		if(field2value==null)
		{
			field2value=new HashMap<String, Object>();
		}
		return field2value;
	}

	public void setField2value(Map<String, Object> field2value) {
		this.field2value = field2value;
	}

	public FilledPackage clone() {
		FilledPackage res = new FilledPackage(_package);
		Map<String, Object> field2valueR = new HashMap<String, Object>();
		if(field2value!=null)
		{
			field2valueR.putAll(field2value);
		}
		res.setField2value(field2valueR);
		return res;
	}

	public FilledPackage makeVariant(String field, Object value) {
		FilledPackage res = clone();
		res.setValue(field, value);
		return res;
	}

	private void setValue(String field, Object value) {
		if (field2value == null)
			field2value = new HashMap<String, Object>();
		field2value.put(field, value);
	}
	
	public PackageTypeWrapper getPackageWrapper() {
		return _package;
	}

	static public List<FilledPackage> generateVariants(
			List<FilledPackage> filledPackages, PackageTypeWrapper _package,
			String fieldName) throws GeneratorException {
		List<FilledPackage> res = new LinkedList<FilledPackage>();
		List<?> values = UniversalValuesSource.getInstance().getValuesForType(
				_package, fieldName);
		if (values.size() < 1)
			throw new GeneratorException("Cannot generate value for field:"
					+ fieldName + " for package: " + _package.getClassName());
		for (Object v : values) {
			for (FilledPackage filledPackage : filledPackages) {
				res.add(filledPackage.makeVariant(fieldName, v));
			}
		}
		return res;
	}

	public void print() {
		System.out.println("Package: "+_package.getClassName());
		print("\t");		
	}
	
	public void print(String deep) {
		for(Map.Entry<String, Object> s:getField2value().entrySet())
		{
			if(FilledPackage.class.isInstance(s.getValue()))
			{
				System.out.println(deep+s.getKey()+":");
				((FilledPackage)s.getValue()).print(deep+"\t");
			}else{
				System.out.println(deep+s.getKey()+":"+s.getValue());
			}
		}		
	}

	public Object getValue(String name) {
		if(field2value!=null)
			return field2value.get(name);
		return null;
	}
	
	public void correctConsistency()
	{
		for(FieldType f:_package.getPackage().getField())
		{
			if(f.getType()==Type.PACKAGE_MAP)
			{
				FilledPackage nested_package=(FilledPackage)getValue(f.getName());
				setValue(f.getObjectRefId(), nested_package.getPackageWrapper().getPackage().getIdValue());
			}
		}
	}
}
