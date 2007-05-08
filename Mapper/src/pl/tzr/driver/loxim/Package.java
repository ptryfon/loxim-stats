package pl.tzr.driver.loxim;

import java.io.InputStream;
import java.io.OutputStream;

import pl.tzr.driver.loxim.exception.SBQLException;



interface Package {
	public static final byte TYPE_SIMPLEQUERY = 0;
	public static final byte TYPE_PARAMQUERY = 1;
	public static final byte TYPE_STATEMENT = 2;
	public static final byte TYPE_PARAMSTATEMENT = 3;
	public static final byte TYPE_SIMPLERESULT = 4;
	public static final byte TYPE_ERRORRESULT = 5;
	
	public short getType();
	
	public int serialize(OutputStream stream) throws SBQLException;
	public void deserialize(InputStream stream, int size) throws SBQLException;
}
