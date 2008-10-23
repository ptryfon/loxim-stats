package pl.edu.mimuw.loxim.protogen.lang.java.template.util;

import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;

public class StreamUtil {
	public static void readNbytes(InputStream is, byte[] res, int n, int offset)
			throws IOException {
		int to_read = n;
		int pos = 0;
		while (to_read > 0) {
			int r = is.read(res, offset + pos, to_read);
			if (r < 0)
				throw new EOFException();
			to_read -= r;
			pos += r;
		}
	}

	public static byte[] readNbytes(InputStream is, int n) throws IOException {
		byte[] res = new byte[n];
		readNbytes(is, res, n, 0);
		return res;
	}

}
