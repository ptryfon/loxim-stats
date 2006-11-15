package loxim.driver;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import loxim.driver.exception.SBQLException;
import loxim.driver.exception.SBQLIOException;


class StatementPackage implements Package {
	long statementNumber = -1;

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
		throw new RuntimeException("Nie powinnismy tego uzywac");
	}

	public long getStatementNumber() {
		return statementNumber;
	}
}
