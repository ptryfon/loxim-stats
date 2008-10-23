
package pl.edu.mimuw.loxim.protocol.enums;

import java.util.EnumSet;

public enum Auth_methodsEnum {

/**
**/
	am_trust((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("0x0001"))),
	/**
autoryzacja metoda stosowana przez serwer MySQL5 - w oparciu o przesy³anie i przechowywanie skrótu has³a algorytmem SHA1. Generalnie schemat jest nastepujacy:
$stringUtil.removeWhiteSpaces($c)

**/
	am_mysql5((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("0x0002")));
		
	private final java.math.BigInteger value;
	private Auth_methodsEnum(java.math.BigInteger val)
	{
		value=val;
	}
	
	public java.math.BigInteger getValue() 
	{
		return value;
	}
	
	public static Auth_methodsEnum createEnumByValue(java.math.BigInteger val)
	{
		for(Auth_methodsEnum v:values())
		{
			if (equals(v.getValue(),val))
				return v;
		}
		return null;
	}
	
	private static boolean equals(long v1,long v2){return v1==v2;};
	private static boolean equals(Object v1,Object v2){return (v1==v2)||(v1!=null && v1.equals(v2)); };
	
	public static java.math.BigInteger toEnumMapValue(EnumSet<Auth_methodsEnum> flags) {
		java.math.BigInteger res=java.math.BigInteger.ZERO;
		for(Auth_methodsEnum flag:flags)
		{
			res=res.or(flag.getValue());
		}
		return res;
}

	public static EnumSet<Auth_methodsEnum> createEnumMapByValue(
			java.math.BigInteger tmp_flags) {
		EnumSet<Auth_methodsEnum> res=EnumSet.noneOf(Auth_methodsEnum.class);
		for(Auth_methodsEnum flag:values())
		{
			if (flag.getValue().and(tmp_flags).equals(flag.getValue()))
				res.add(flag);
		}
		return res;		
	}
	
		/* for test purposes */
	static EnumSet<Auth_methodsEnum> ODD = null;

	public static EnumSet<Auth_methodsEnum> getODD() {
		if (ODD == null) {
			boolean odd = true;
			ODD=EnumSet.noneOf(Auth_methodsEnum.class);
			for (Auth_methodsEnum v : values()) {
				if(odd) ODD.add(v);
				odd = !odd;
			}
		}
		return ODD;
	}
}

