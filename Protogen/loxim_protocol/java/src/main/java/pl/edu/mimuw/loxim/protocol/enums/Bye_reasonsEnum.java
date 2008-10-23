
package pl.edu.mimuw.loxim.protocol.enums;

import java.util.EnumSet;

public enum Bye_reasonsEnum {

/**
**/
	br_reason1((long)0x0001);
		
	private final long value;
	private Bye_reasonsEnum(long val)
	{
		value=val;
	}
	
	public long getValue() 
	{
		return value;
	}
	
	public static Bye_reasonsEnum createEnumByValue(long val)
	{
		for(Bye_reasonsEnum v:values())
		{
			if (equals(v.getValue(),val))
				return v;
		}
		return null;
	}
	
	private static boolean equals(long v1,long v2){return v1==v2;};
	private static boolean equals(Object v1,Object v2){return (v1==v2)||(v1!=null && v1.equals(v2)); };
	
	public static long toEnumMapValue(EnumSet<Bye_reasonsEnum> flags) {
		long res=0;
		for(Bye_reasonsEnum flag:flags)
		{
			res=(long)(res|flag.getValue());
		}
		return res;
}

	public static EnumSet<Bye_reasonsEnum> createEnumMapByValue(
			long tmp_flags) {
		EnumSet<Bye_reasonsEnum> res=EnumSet.noneOf(Bye_reasonsEnum.class);
		for(Bye_reasonsEnum flag:values())
		{
			if ((flag.getValue() & tmp_flags) == flag.getValue())
				res.add(flag);
		}
		return res;		
	}
	
		/* for test purposes */
	static EnumSet<Bye_reasonsEnum> ODD = null;

	public static EnumSet<Bye_reasonsEnum> getODD() {
		if (ODD == null) {
			boolean odd = true;
			ODD=EnumSet.noneOf(Bye_reasonsEnum.class);
			for (Bye_reasonsEnum v : values()) {
				if(odd) ODD.add(v);
				odd = !odd;
			}
		}
		return ODD;
	}
}

