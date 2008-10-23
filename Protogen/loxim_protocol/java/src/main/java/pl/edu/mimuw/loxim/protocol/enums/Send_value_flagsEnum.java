
package pl.edu.mimuw.loxim.protocol.enums;

import java.util.EnumSet;

public enum Send_value_flagsEnum {

/**
**/
	svf_to_be_continued((short)0x01);
		
	private final short value;
	private Send_value_flagsEnum(short val)
	{
		value=val;
	}
	
	public short getValue() 
	{
		return value;
	}
	
	public static Send_value_flagsEnum createEnumByValue(short val)
	{
		for(Send_value_flagsEnum v:values())
		{
			if (equals(v.getValue(),val))
				return v;
		}
		return null;
	}
	
	private static boolean equals(long v1,long v2){return v1==v2;};
	private static boolean equals(Object v1,Object v2){return (v1==v2)||(v1!=null && v1.equals(v2)); };
	
	public static short toEnumMapValue(EnumSet<Send_value_flagsEnum> flags) {
		short res=0;
		for(Send_value_flagsEnum flag:flags)
		{
			res=(short)(res|flag.getValue());
		}
		return res;
}

	public static EnumSet<Send_value_flagsEnum> createEnumMapByValue(
			short tmp_flags) {
		EnumSet<Send_value_flagsEnum> res=EnumSet.noneOf(Send_value_flagsEnum.class);
		for(Send_value_flagsEnum flag:values())
		{
			if ((flag.getValue() & tmp_flags) == flag.getValue())
				res.add(flag);
		}
		return res;		
	}
	
		/* for test purposes */
	static EnumSet<Send_value_flagsEnum> ODD = null;

	public static EnumSet<Send_value_flagsEnum> getODD() {
		if (ODD == null) {
			boolean odd = true;
			ODD=EnumSet.noneOf(Send_value_flagsEnum.class);
			for (Send_value_flagsEnum v : values()) {
				if(odd) ODD.add(v);
				odd = !odd;
			}
		}
		return ODD;
	}
}

