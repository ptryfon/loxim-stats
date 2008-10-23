package pl.edu.mimuw.loxim.protogen.lang.java.hlp;

public class StringUtil {
	public static String removeWhiteSpaces(String s)
	{
		return s.trim().replaceAll("\\s+"," ");
	}
}
