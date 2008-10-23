package pl.edu.mimuw.loxim.protogen.lang.java.template.util;

import java.math.BigInteger;

public class TypeUtil {
	public static BigInteger str2bigInteger(String s)
	{
		if ((s.length()>2)&&(s.startsWith("0x")))
		{
			return BigInteger.valueOf(Long.parseLong(s.substring(2),16));
		} else 
		{
			return new BigInteger(s);
		}
	}
}
