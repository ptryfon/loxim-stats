package loxim.driver;

import java.io.DataInputStream;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;

class ResultStream extends InputStream {
	long length;
	int pos = 0;
	DataInputStream is;
	
	public ResultStream(DataInputStream dis) throws IOException {
		byte b[] = new byte[4];
		
		dis.read(b);
		length = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
		
		pos = 0;
		is = dis;		
	}
	
	public long readULong() throws IOException {
		short s[] = new short[4];
		for (int i=0; i< 4; i++) s[i] = (short)read();
		long res = (s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3];
		return res;
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
