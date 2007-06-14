package pl.tzr.driver.loxim;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.exception.SBQLIOException;
import pl.tzr.driver.loxim.exception.SBQLProtocolException;
import pl.tzr.driver.loxim.result.Result;
import pl.tzr.driver.loxim.result.ResultBag;
import pl.tzr.driver.loxim.result.ResultBinder;
import pl.tzr.driver.loxim.result.ResultBool;
import pl.tzr.driver.loxim.result.ResultCollection;
import pl.tzr.driver.loxim.result.ResultDouble;
import pl.tzr.driver.loxim.result.ResultError;
import pl.tzr.driver.loxim.result.ResultInt;
import pl.tzr.driver.loxim.result.ResultReference;
import pl.tzr.driver.loxim.result.ResultSequence;
import pl.tzr.driver.loxim.result.ResultString;
import pl.tzr.driver.loxim.result.ResultStruct;
import pl.tzr.driver.loxim.result.ResultType;
import pl.tzr.driver.loxim.result.ResultVoid;



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
		throw new UnsupportedOperationException("Not to be used on the client side");
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
				throw new SBQLProtocolException("Unexpected result type (" + objType + ")");
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
