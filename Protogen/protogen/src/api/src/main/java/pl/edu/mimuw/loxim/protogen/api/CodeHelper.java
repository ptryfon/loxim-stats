package pl.edu.mimuw.loxim.protogen.api;


public class CodeHelper {


	public static String upperFirstChar(String str) {
		return str.length()>0?Character.toUpperCase(str.charAt(0))+str.substring(1):"";
	}
}
