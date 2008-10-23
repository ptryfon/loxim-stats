
package pl.edu.mimuw.loxim.protocol.enums;

import java.util.EnumSet;

public enum Abort_reason_codesEnum {

/**
**/
	arc_administration_reason((long)1),
	/**
**/
	arc_transaction_victim((long)2),
	/**
**/
	arc_operation_not_permited((long)3),
	/**
**/
	arc_time_limit_exceeded((long)4),
	/**
**/
	arc_out_of_memory((long)5),
	/**
**/
	arc_type_check_error((long)6),
	/**
**/
	arc_other_run_time_error((long)7);
		
	private final long value;
	private Abort_reason_codesEnum(long val)
	{
		value=val;
	}
	
	public long getValue() 
	{
		return value;
	}
	
	public static Abort_reason_codesEnum createEnumByValue(long val)
	{
		for(Abort_reason_codesEnum v:values())
		{
			if (equals(v.getValue(),val))
				return v;
		}
		return null;
	}
	
	private static boolean equals(long v1,long v2){return v1==v2;};
	private static boolean equals(Object v1,Object v2){return (v1==v2)||(v1!=null && v1.equals(v2)); };
	
	public static long toEnumMapValue(EnumSet<Abort_reason_codesEnum> flags) {
		long res=0;
		for(Abort_reason_codesEnum flag:flags)
		{
			res=(long)(res|flag.getValue());
		}
		return res;
}

	public static EnumSet<Abort_reason_codesEnum> createEnumMapByValue(
			long tmp_flags) {
		EnumSet<Abort_reason_codesEnum> res=EnumSet.noneOf(Abort_reason_codesEnum.class);
		for(Abort_reason_codesEnum flag:values())
		{
			if ((flag.getValue() & tmp_flags) == flag.getValue())
				res.add(flag);
		}
		return res;		
	}
	
		/* for test purposes */
	static EnumSet<Abort_reason_codesEnum> ODD = null;

	public static EnumSet<Abort_reason_codesEnum> getODD() {
		if (ODD == null) {
			boolean odd = true;
			ODD=EnumSet.noneOf(Abort_reason_codesEnum.class);
			for (Abort_reason_codesEnum v : values()) {
				if(odd) ODD.add(v);
				odd = !odd;
			}
		}
		return ODD;
	}
}

