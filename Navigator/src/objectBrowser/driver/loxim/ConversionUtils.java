package objectBrowser.driver.loxim;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class ConversionUtils {
	public static void writeUInt(OutputStream s, long i) throws IOException {
		s.write((int)(i >> 24));
		s.write((int)(i >> 16));
		s.write((int)(i >> 8));
		s.write((int)(i % 256));
	}
	
	public static long readUInt(InputStream stream) throws IOException {		
		short s[] = new short[4];
		for (int i=0; i< 4; i++) s[i] = (short)stream.read();
		long res = (s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3];
		return res;		
	}
	
	public static void writeEnum(OutputStream s, short i) throws IOException {
		s.write(i);
	}
	
	public static short readEnum(InputStream s) throws IOException{
		return (short)s.read();
	}	
	
	public static int getEnumSize() {
		return 1;
	}
}
