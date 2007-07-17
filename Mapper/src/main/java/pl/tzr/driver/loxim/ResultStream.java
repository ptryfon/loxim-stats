package pl.tzr.driver.loxim;

import java.io.DataInputStream;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;

class ResultStream extends InputStream {
	
	long length;
	int pos = 0;
	DataInputStream is;
	
	public ResultStream(DataInputStream dis) throws IOException {
		length = ConversionUtils.readUInt(dis);
		
		pos = 0;
		is = dis;		
	}
	
	@Override
	public int read() throws IOException {
		if (pos >= length) {			
			throw new EOFException();
		}
		pos++;
		return is.read();
	}

	public void flush() throws IOException {
		while (pos < length) read();
	}
}
