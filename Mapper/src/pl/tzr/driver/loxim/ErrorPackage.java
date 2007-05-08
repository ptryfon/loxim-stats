package pl.tzr.driver.loxim;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.exception.SBQLIOException;

public class ErrorPackage implements Package {
	
	private long error;

	public void deserialize(InputStream stream, int size) throws SBQLException {
		
		try {
		
		error = ConversionUtils.readUInt(stream);
		
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}

	}

	public short getType() {
		return Package.TYPE_ERRORRESULT;
	}

	public int serialize(OutputStream stream) throws SBQLException {
		
		throw new UnsupportedOperationException();
		
	}

	public long getError() {
		return error;
	}

}
