package pl.tzr.driver.loxim;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Map;
import java.util.Map.Entry;

import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.exception.SBQLIOException;
import pl.tzr.driver.loxim.exception.SBQLProtocolException;
import pl.tzr.driver.loxim.result.Result;
import pl.tzr.driver.loxim.result.ResultBinder;
import pl.tzr.driver.loxim.result.ResultBool;
import pl.tzr.driver.loxim.result.ResultCollection;
import pl.tzr.driver.loxim.result.ResultDouble;
import pl.tzr.driver.loxim.result.ResultError;
import pl.tzr.driver.loxim.result.ResultInt;
import pl.tzr.driver.loxim.result.ResultReference;
import pl.tzr.driver.loxim.result.ResultString;
import pl.tzr.driver.loxim.result.ResultVoid;



class ParamStatementPackage implements Package {
	
	long statementNumber;
	
	Map<String, Result> params;
	
	public ParamStatementPackage() {
        /* Default constructor */
	}
	
	public ParamStatementPackage(long statementNumber, Map<String, Result> params) {
		this.statementNumber = statementNumber;
		this.params = params;
	}
	
	private void serializeResultCollection(
            OutputStream stream, 
            ResultCollection rc) throws SBQLException {
        
		try {
			ConversionUtils.writeUInt(stream, rc.getItems().size());
			for (Result item : rc.getItems()) {
				serializeResult(stream, item);
			}
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}
	}
	
	private void serializeString(OutputStream stream, String s) throws SBQLException{
		try {
			ConversionUtils.writeUInt(stream, s.length() + 1);
			ConversionUtils.writeString(stream, s);
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}
	}
	
	private void serializeResult(OutputStream stream, Result result) throws SBQLException {
		try {
			ConversionUtils.writeUInt(stream, result.getType());
			if (result instanceof ResultBinder) {
				serializeString(stream, ((ResultBinder)result).getKey());
				serializeResult(stream, ((ResultBinder)result).getValue());				
			} else if (result instanceof ResultCollection) {
				serializeResultCollection(stream, (ResultCollection)result);
			} else if (result instanceof ResultReference) {
				serializeString(stream, ((ResultReference)result).getRef());
			} else if (result instanceof ResultVoid) {
				/* length 0 */
			} else if (result instanceof ResultString) {
				serializeString(stream, ((ResultString)result).getValue());
			} else if (result instanceof ResultError) {
				ConversionUtils.writeUInt(stream, ((ResultError)result).getErrorCode());
			} else if (result instanceof ResultInt) {
				ConversionUtils.writeUInt(stream, ((ResultInt)result).getValue());
			} else if (result instanceof ResultBool) {
				if (((ResultBool)result).getValue()) {
					ConversionUtils.writeUInt(stream, 1);
				} else {
					ConversionUtils.writeUInt(stream, 0);
				}
			} else if (result instanceof ResultDouble) {
				ConversionUtils.writeDouble(stream, ((ResultDouble)result).getValue());
			} else throw new SBQLProtocolException("Unknown package type");
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}
	}
	
	private int getResultCollectionLength(ResultCollection result) throws SBQLException {
		int length = ConversionUtils.getUIntSize();
		for (Result item : result.getItems()) {
			length += getResultLength(item);
		}
		return length;
	}
	
	private int getResultLength(Result result) throws SBQLException {
		int length = ConversionUtils.getUIntSize();  //Result type
		
		if (result instanceof ResultBinder) {
			ResultBinder binder = (ResultBinder)result;
			length += binder.getKey().length() + 1;
			length += ConversionUtils.getUIntSize();
			length += getResultLength(binder.getValue());
		} else if (result instanceof ResultCollection) {
			length  += getResultCollectionLength((ResultCollection)result);
		} else if (result instanceof ResultReference) {
			int refLen = ((ResultReference)result).getRef().length() + 1;
			length += ConversionUtils.getUIntSize() + refLen;
		} else if (result instanceof ResultVoid) {
			/* dlugosc 0 */
		} else if (result instanceof ResultString) {
			int strLen = ((ResultString)result).getValue().length() + 1;
			length += ConversionUtils.getUIntSize() + strLen;			
		} else if (result instanceof ResultError) {
			length += ConversionUtils.getUIntSize();
		} else if (result instanceof ResultInt) {
			length += ConversionUtils.getUIntSize();
		} else if (result instanceof ResultBool) {
			length += ConversionUtils.getUIntSize();
		} else if (result instanceof ResultDouble) {
			length += ConversionUtils.getDoubleSize();
		} else throw new SBQLProtocolException("Unknown package type");
		return length; 
	}

	public void deserialize(InputStream stream, int size) {
		throw new RuntimeException("Incorrect use of driver");
	}

	public short getType() {
		return Package.TYPE_PARAMSTATEMENT;
	}

	public int serialize(OutputStream stream) throws SBQLException {
		int length;
		length = ConversionUtils.getEnumSize(); //Package type
		length += ConversionUtils.getUIntSize(); //Total length
		length += ConversionUtils.getUIntSize(); //Prepared query numer
		length += ConversionUtils.getUIntSize(); //Parameter count
		
		
		for (Entry<String, Result> param : params.entrySet()) {
		    /* Parameter name - with byte #0 on the end */
			length += param.getKey().length() + 1;
            
            /* Parameter value */
			length += getResultLength(param.getValue());
            
            /* ??? */
			length += ConversionUtils.getUIntSize(); 
		}
		
		try {
			ConversionUtils.writeEnum(stream, getType());
			ConversionUtils.writeUInt(stream, length);
			ConversionUtils.writeUInt(stream, statementNumber);
			ConversionUtils.writeUInt(stream, params.size());
			
			for (Entry<String, Result> param : params.entrySet()) {
				serializeString(stream, param.getKey());
				serializeResult(stream, param.getValue());
			}
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}
		
		return length;
	}

	public long getStatementNumber() {
		return statementNumber;
	}

	public void setStatementNumber(long statementNumber) {
		this.statementNumber = statementNumber;
	}

	public Map<String, Result> getParams() {
		return params;
	}

	public void setParams(Map<String, Result> params) {
		this.params = params;
	}

}
