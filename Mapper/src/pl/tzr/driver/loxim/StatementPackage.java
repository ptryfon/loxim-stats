package pl.tzr.driver.loxim;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.exception.SBQLIOException;



class StatementPackage implements Package {
	private long statementNumber = -1;

	public void deserialize(InputStream stream, int size) throws SBQLException,
			SBQLException {
		try {
			statementNumber = ConversionUtils.readUInt(stream);
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}

	}

	public short getType() {
		return Package.TYPE_STATEMENT;
	}

	public int serialize(OutputStream stream) throws SBQLException {
		throw new UnsupportedOperationException("Not to be used on the client side");
	}

	public long getStatementNumber() {		
		return statementNumber;
	}
}
