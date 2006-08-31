package objectBrowser.driver.loxim;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.sun.corba.se.spi.legacy.connection.GetEndPointInfoAgainException;

public class SimpleQueryPackage implements Package {
	protected String query;

	public short getType() {
		return Package.TYPE_SIMPLEQUERY;
	}

	public int serialize(OutputStream stream) throws IOException {
		ConversionUtils.writeEnum(stream, getType());
		stream.write(query.getBytes());
		stream.write(0);
		int size = query.length() + ConversionUtils.getEnumSize() + 1;
		
		return size;
	}

	public void deserialize(InputStream stream, int size) throws IOException {
		short type = ConversionUtils.readEnum(stream);
		byte[] chars = new byte[size - ConversionUtils.getEnumSize() - 1];
		stream.read(chars);
		query = new String(chars);
		
		stream.read(new byte[1]); /* czytamy bajt o warto¶ci 0 */
		
		if (type != getType()) throw new IOException("Incorrect package type");		
	}

	public String getQuery() {
		return query;
	}

	public void setQuery(String query) {
		this.query = query;
	}

}
