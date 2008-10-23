
package pl.edu.mimuw.loxim.protocol.enums;

import java.util.EnumSet;

public enum Statement_flagsEnum {

/**
**/
	sf_execute((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("0x0001"))),
	/**
**/
	sf_readonly((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("0x0002"))),
	/**
**/
	sf_prefer_dfs((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("0x0010"))),
	/**
**/
	sf_prefer_bsf((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("0x0020")));
		
	private final java.math.BigInteger value;
	private Statement_flagsEnum(java.math.BigInteger val)
	{
		value=val;
	}
	
	public java.math.BigInteger getValue() 
	{
		return value;
	}
	
	public static Statement_flagsEnum createEnumByValue(java.math.BigInteger val)
	{
		for(Statement_flagsEnum v:values())
		{
			if (equals(v.getValue(),val))
				return v;
		}
		return null;
	}
	
	private static boolean equals(long v1,long v2){return v1==v2;};
	private static boolean equals(Object v1,Object v2){return (v1==v2)||(v1!=null && v1.equals(v2)); };
	
	public static java.math.BigInteger toEnumMapValue(EnumSet<Statement_flagsEnum> flags) {
		java.math.BigInteger res=java.math.BigInteger.ZERO;
		for(Statement_flagsEnum flag:flags)
		{
			res=res.or(flag.getValue());
		}
		return res;
}

	public static EnumSet<Statement_flagsEnum> createEnumMapByValue(
			java.math.BigInteger tmp_flags) {
		EnumSet<Statement_flagsEnum> res=EnumSet.noneOf(Statement_flagsEnum.class);
		for(Statement_flagsEnum flag:values())
		{
			if (flag.getValue().and(tmp_flags).equals(flag.getValue()))
				res.add(flag);
		}
		return res;		
	}
	
		/* for test purposes */
	static EnumSet<Statement_flagsEnum> ODD = null;

	public static EnumSet<Statement_flagsEnum> getODD() {
		if (ODD == null) {
			boolean odd = true;
			ODD=EnumSet.noneOf(Statement_flagsEnum.class);
			for (Statement_flagsEnum v : values()) {
				if(odd) ODD.add(v);
				odd = !odd;
			}
		}
		return ODD;
	}
}

