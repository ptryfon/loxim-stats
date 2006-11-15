package loxim.driver;

import java.io.InputStream;
import java.io.OutputStream;

import loxim.driver.exception.SBQLException;


interface Package {
	public static final byte TYPE_SIMPLEQUERY = 0;
	public static final byte TYPE_PARAMQUERY = 1;
	public static final byte TYPE_STATEMENT = 2;
	public static final byte TYPE_PARAMSTATEMENT = 3;
	public static final byte TYPE_SIMPLERESULT = 4;
	
	public short getType();
	
	public int serialize(OutputStream stream) throws SBQLException;
	public void deserialize(InputStream stream, int size) throws SBQLException;
}
