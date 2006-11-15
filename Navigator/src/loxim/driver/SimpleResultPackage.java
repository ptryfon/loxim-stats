package loxim.driver;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import loxim.driver.exception.SBQLException;
import loxim.driver.exception.SBQLIOException;
import loxim.driver.exception.SBQLProtocolException;
import loxim.driver.result.Result;
import loxim.driver.result.ResultBag;
import loxim.driver.result.ResultBinder;
import loxim.driver.result.ResultBool;
import loxim.driver.result.ResultCollection;
import loxim.driver.result.ResultDouble;
import loxim.driver.result.ResultError;
import loxim.driver.result.ResultInt;
import loxim.driver.result.ResultReference;
import loxim.driver.result.ResultSequence;
import loxim.driver.result.ResultString;
import loxim.driver.result.ResultStruct;
import loxim.driver.result.ResultType;
import loxim.driver.result.ResultVoid;


class SimpleResultPackage implements Package {
	private Result result;
	
	

	public short getType() {		
		return Package.TYPE_SIMPLERESULT;
	}
	
	public void setResult(Result res) {
		result = res;		
	}

	public Result getResult() {
		return result;
	}
	
	public int serialize(OutputStream stream) throws SBQLException {
		throw new RuntimeException("Funkcja niezimplementowana");
	}

	public void deserialize(InputStream stream, int size) throws SBQLException {
			result = deserializeResult(stream);
	}

	protected Result deserializeResult(InputStream is) throws SBQLException {
		try {
			byte objType = (byte) is.read();
			switch (objType) {
			case ResultType.RES_BAG:
				return deserializeCollection(is, new ResultBag());
			case ResultType.RES_SEQUENCE:
				return deserializeCollection(is, new ResultSequence());
			case ResultType.RES_STRUCT:
				return deserializeCollection(is, new ResultStruct());
			case ResultType.RES_REFERENCE:
				return new ResultReference(deserializeString(is));
			case ResultType.RES_VOID:
				return new ResultVoid();
			case ResultType.RES_STRING:
				return new ResultString(deserializeString(is));
			case ResultType.RES_ERROR:
				return new ResultError(ConversionUtils.readUInt(is));
			case ResultType.RES_INT:
				return new ResultInt((int) ConversionUtils.readUInt(is));
			case ResultType.RES_BOOLTRUE:
				return new ResultBool(true);
			case ResultType.RES_BOOLFALSE:
				return new ResultBool(false);
			case ResultType.RES_DOUBLE:
				return new ResultDouble(ConversionUtils.readDouble(is));
			case ResultType.RES_BINDER:
				return new ResultBinder(deserializeString(is), deserializeResult(is));
			default:
				throw new SBQLProtocolException("Nieznany rodzaj wyniku (" + objType + ")");
			}
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}
	}
	
	protected String deserializeString(InputStream is) throws SBQLException {
		try 
		{
		String s = "";
		char c = (char) is.read();
		while (c != 0) {
			s = s + c;
			c = (char) is.read();
		}
		return s;
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}
	}	
	
	protected Result deserializeCollection(InputStream is, ResultCollection col)
	throws IOException, SBQLException {
		long count = ConversionUtils.readUInt(is);
		for (int i = 0; i < count; i++) {
			col.addItem(deserializeResult(is));
		}
		return col;
	}
	
	
}
