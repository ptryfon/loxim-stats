
package pl.edu.mimuw.loxim.protocol.enums;

import java.util.EnumSet;

public enum CollationsEnum {

/**
**/
	coll_default((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("0"))),
	/**
Unicode Collation Algorithm (http://unicode.org/unicode/reports/tr10)
**/
	coll_unicode_collation_alghorithm_uts10((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("1")));
		
	private final java.math.BigInteger value;
	private CollationsEnum(java.math.BigInteger val)
	{
		value=val;
	}
	
	public java.math.BigInteger getValue() 
	{
		return value;
	}
	
	public static CollationsEnum createEnumByValue(java.math.BigInteger val)
	{
		for(CollationsEnum v:values())
		{
			if (equals(v.getValue(),val))
				return v;
		}
		return null;
	}
	
	private static boolean equals(long v1,long v2){return v1==v2;};
	private static boolean equals(Object v1,Object v2){return (v1==v2)||(v1!=null && v1.equals(v2)); };
	
	public static java.math.BigInteger toEnumMapValue(EnumSet<CollationsEnum> flags) {
		java.math.BigInteger res=java.math.BigInteger.ZERO;
		for(CollationsEnum flag:flags)
		{
			res=res.or(flag.getValue());
		}
		return res;
}

	public static EnumSet<CollationsEnum> createEnumMapByValue(
			java.math.BigInteger tmp_flags) {
		EnumSet<CollationsEnum> res=EnumSet.noneOf(CollationsEnum.class);
		for(CollationsEnum flag:values())
		{
			if (flag.getValue().and(tmp_flags).equals(flag.getValue()))
				res.add(flag);
		}
		return res;		
	}
	
		/* for test purposes */
	static EnumSet<CollationsEnum> ODD = null;

	public static EnumSet<CollationsEnum> getODD() {
		if (ODD == null) {
			boolean odd = true;
			ODD=EnumSet.noneOf(CollationsEnum.class);
			for (CollationsEnum v : values()) {
				if(odd) ODD.add(v);
				odd = !odd;
			}
		}
		return ODD;
	}
}

