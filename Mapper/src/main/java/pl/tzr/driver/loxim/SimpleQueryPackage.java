package pl.tzr.driver.loxim;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.exception.SBQLIOException;


class SimpleQueryPackage implements Package {
    
	protected String query;
	
	public SimpleQueryPackage(String query) {
		this.query = query;
	}
	
	public SimpleQueryPackage() {
        /* Default constructor */
	}

	public short getType() {
		return Package.TYPE_SIMPLEQUERY;
	}

	public int serialize(OutputStream stream) throws SBQLException {
		try {
			ConversionUtils.writeEnum(stream, getType());
			stream.write(query.getBytes());
			stream.write(0);
			int size = query.length() + ConversionUtils.getEnumSize() + 1;
			return size;
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}
		
		
	}

	public void deserialize(InputStream stream, int size) throws SBQLException {
		try {
			byte[] chars = new byte[size - ConversionUtils.getEnumSize() - 1];
			stream.read(chars);
			query = new String(chars);
			
			int b = stream.read(); /* czytamy bajt o warto�ci 0 */
			assert(b == 0);
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}
	}

	public String getQuery() {
		return query;
	}

	public void setQuery(String query) {
		this.query = query;
	}

}
