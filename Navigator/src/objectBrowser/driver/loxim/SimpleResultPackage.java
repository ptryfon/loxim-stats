package objectBrowser.driver.loxim;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import objectBrowser.driver.loxim.result.Result;
import objectBrowser.driver.loxim.result.ResultBag;
import objectBrowser.driver.loxim.result.ResultBinder;
import objectBrowser.driver.loxim.result.ResultBool;
import objectBrowser.driver.loxim.result.ResultCollection;
import objectBrowser.driver.loxim.result.ResultError;
import objectBrowser.driver.loxim.result.ResultInt;
import objectBrowser.driver.loxim.result.ResultReference;
import objectBrowser.driver.loxim.result.ResultSequence;
import objectBrowser.driver.loxim.result.ResultString;
import objectBrowser.driver.loxim.result.ResultStruct;
import objectBrowser.driver.loxim.result.ResultType;
import objectBrowser.driver.loxim.result.ResultVoid;

public class SimpleResultPackage implements Package {
	private Result result;
	
	static final byte RES_RESULT = 0;

	static final byte RES_SEQUENCE = 1;

	static final byte RES_BAG = 2;

	static final byte RES_STRUCT = 3;

	static final byte RES_BINDER = 4;

	static final byte RES_STRING = 5;

	static final byte RES_INT = 6;

	static final byte RES_DOUBLE = 7;

	static final byte RES_BOOL = 8;

	static final byte RES_REFERENCE = 9;

	static final byte RES_VOID = 10;

	static final byte RES_ERROR = 11;

	static final byte RES_BOOLTRUE = 12;

	static final byte RES_BOOLFALSE = 13;
	

	public short getType() {		
		return Package.TYPE_SIMPLERESULT;
	}
	
	public void setResult(Result res) {
		result = res;		
	}

	public Result getResult() {
		return result;
	}
	
	public int serialize(OutputStream stream) throws IOException {
		
		// TODO - póki co niepotrzebne
		return 0;
	}

	public void deserialize(InputStream stream, int size) throws IOException, SBQLException {
			short type = ConversionUtils.readEnum(stream);
			if (type != getType()) throw new IOException("Incorrect package type");
			result = dataDeserialize(stream);
	}

	protected Result dataDeserialize(InputStream is) throws IOException, SBQLException {
		byte objType = (byte) is.read();
		switch (objType) {
		case RES_BAG:
			return grabElements(is, new ResultBag());
		case RES_SEQUENCE:
			return grabElements(is, new ResultSequence());
		case RES_STRUCT:
			return grabElements(is, new ResultStruct());
		case RES_REFERENCE:
			return new ResultReference(grabString(is));
		case RES_VOID:
			return new ResultVoid();
		case RES_STRING:
			return new ResultString(grabString(is));
		case RES_ERROR:
			return new ResultError(ConversionUtils.readUInt(is));
		case RES_INT:
			return new ResultInt((int) ConversionUtils.readUInt(is));
		case RES_BOOLTRUE:
			return new ResultBool(true);
		case RES_BOOLFALSE:
			return new ResultBool(false);
		case RES_DOUBLE:
			return null; /* TODO */
		case RES_BINDER:
			return new ResultBinder(grabString(is), dataDeserialize(is));
		default:
			throw new SBQLException("Unknown result type (" + objType + ")",
					null);
		}
	}
	
	String grabString(InputStream is) throws IOException {
		String s = "";
		char c = (char) is.read();
		while (c != 0) {
			s = s + c;
			c = (char) is.read();
		}
		return s;
	}	
	
	Result grabElements(InputStream is, ResultCollection col)
	throws IOException, SBQLException {
		long count = ConversionUtils.readUInt(is);
		for (int i = 0; i < count; i++) {
			col.addItem(dataDeserialize(is));
		}
		return col;
	}
	
	
}
