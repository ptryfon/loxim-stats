
package pl.edu.mimuw.loxim.protocol.enums;

import java.util.EnumSet;

public enum FeaturesEnum {

/**
Po³aczenie moze byc szyfrowane metoda SSL
**/
	f_ssl((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("0x0001"))),
	/**
Obligatoryjne po³aczenie szyfrowane metoda SSL (wymusza tez obecnosc flagi F SSL)
**/
	f_o_ssl((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("0x0002"))),
	/**
po³aczenie mo¿e byæ kompresowane za pomoc± biblioteki ZLIB
**/
	f_zlib((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("0x0004"))),
	/**
serwer udostepnia tryb autocommit (kazde zapytanie zadane poza transakcja rozpoczyna swoja transakcje, które jest automatycznie zamykana po wykonaniu polecenia)
**/
	f_autocommit((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("0x0010"))),
	/**
mozna w³aczyc optymalizator zapytañ...
**/
	f_optimization((pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger("0x0020")));
		
	private final java.math.BigInteger value;
	private FeaturesEnum(java.math.BigInteger val)
	{
		value=val;
	}
	
	public java.math.BigInteger getValue() 
	{
		return value;
	}
	
	public static FeaturesEnum createEnumByValue(java.math.BigInteger val)
	{
		for(FeaturesEnum v:values())
		{
			if (equals(v.getValue(),val))
				return v;
		}
		return null;
	}
	
	private static boolean equals(long v1,long v2){return v1==v2;};
	private static boolean equals(Object v1,Object v2){return (v1==v2)||(v1!=null && v1.equals(v2)); };
	
	public static java.math.BigInteger toEnumMapValue(EnumSet<FeaturesEnum> flags) {
		java.math.BigInteger res=java.math.BigInteger.ZERO;
		for(FeaturesEnum flag:flags)
		{
			res=res.or(flag.getValue());
		}
		return res;
}

	public static EnumSet<FeaturesEnum> createEnumMapByValue(
			java.math.BigInteger tmp_flags) {
		EnumSet<FeaturesEnum> res=EnumSet.noneOf(FeaturesEnum.class);
		for(FeaturesEnum flag:values())
		{
			if (flag.getValue().and(tmp_flags).equals(flag.getValue()))
				res.add(flag);
		}
		return res;		
	}
	
		/* for test purposes */
	static EnumSet<FeaturesEnum> ODD = null;

	public static EnumSet<FeaturesEnum> getODD() {
		if (ODD == null) {
			boolean odd = true;
			ODD=EnumSet.noneOf(FeaturesEnum.class);
			for (FeaturesEnum v : values()) {
				if(odd) ODD.add(v);
				odd = !odd;
			}
		}
		return ODD;
	}
}

