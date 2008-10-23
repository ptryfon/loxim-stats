
package pl.edu.mimuw.loxim.protocol.enums;

import java.util.EnumSet;

public enum ModesEnum {

/**
**/
	mode_ssl((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("1"))),
	/**
**/
	mode_zlib((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("2")));
		
	private final java.math.BigInteger value;
	private ModesEnum(java.math.BigInteger val)
	{
		value=val;
	}
	
	public java.math.BigInteger getValue() 
	{
		return value;
	}
	
	public static ModesEnum createEnumByValue(java.math.BigInteger val)
	{
		for(ModesEnum v:values())
		{
			if (equals(v.getValue(),val))
				return v;
		}
		return null;
	}
	
	private static boolean equals(long v1,long v2){return v1==v2;};
	private static boolean equals(Object v1,Object v2){return (v1==v2)||(v1!=null && v1.equals(v2)); };
	
	public static java.math.BigInteger toEnumMapValue(EnumSet<ModesEnum> flags) {
		java.math.BigInteger res=java.math.BigInteger.ZERO;
		for(ModesEnum flag:flags)
		{
			res=res.or(flag.getValue());
		}
		return res;
}

	public static EnumSet<ModesEnum> createEnumMapByValue(
			java.math.BigInteger tmp_flags) {
		EnumSet<ModesEnum> res=EnumSet.noneOf(ModesEnum.class);
		for(ModesEnum flag:values())
		{
			if (flag.getValue().and(tmp_flags).equals(flag.getValue()))
				res.add(flag);
		}
		return res;		
	}
	
		/* for test purposes */
	static EnumSet<ModesEnum> ODD = null;

	public static EnumSet<ModesEnum> getODD() {
		if (ODD == null) {
			boolean odd = true;
			ODD=EnumSet.noneOf(ModesEnum.class);
			for (ModesEnum v : values()) {
				if(odd) ODD.add(v);
				odd = !odd;
			}
		}
		return ODD;
	}
}

