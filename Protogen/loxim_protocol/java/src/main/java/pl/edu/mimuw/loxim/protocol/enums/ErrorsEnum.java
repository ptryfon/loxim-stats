
package pl.edu.mimuw.loxim.protocol.enums;

import java.util.EnumSet;

public enum ErrorsEnum {

/**
**/
	error_internal((long)0x0000),
	/**
**/
	error_params_incomplete((long)0x0001),
	/**
**/
	error_no_such_value_id((long)0x0002),
	/**
**/
	error_operation_not_permited((long)0x0003);
		
	private final long value;
	private ErrorsEnum(long val)
	{
		value=val;
	}
	
	public long getValue() 
	{
		return value;
	}
	
	public static ErrorsEnum createEnumByValue(long val)
	{
		for(ErrorsEnum v:values())
		{
			if (equals(v.getValue(),val))
				return v;
		}
		return null;
	}
	
	private static boolean equals(long v1,long v2){return v1==v2;};
	private static boolean equals(Object v1,Object v2){return (v1==v2)||(v1!=null && v1.equals(v2)); };
	
	public static long toEnumMapValue(EnumSet<ErrorsEnum> flags) {
		long res=0;
		for(ErrorsEnum flag:flags)
		{
			res=(long)(res|flag.getValue());
		}
		return res;
}

	public static EnumSet<ErrorsEnum> createEnumMapByValue(
			long tmp_flags) {
		EnumSet<ErrorsEnum> res=EnumSet.noneOf(ErrorsEnum.class);
		for(ErrorsEnum flag:values())
		{
			if ((flag.getValue() & tmp_flags) == flag.getValue())
				res.add(flag);
		}
		return res;		
	}
	
		/* for test purposes */
	static EnumSet<ErrorsEnum> ODD = null;

	public static EnumSet<ErrorsEnum> getODD() {
		if (ODD == null) {
			boolean odd = true;
			ODD=EnumSet.noneOf(ErrorsEnum.class);
			for (ErrorsEnum v : values()) {
				if(odd) ODD.add(v);
				odd = !odd;
			}
		}
		return ODD;
	}
}

