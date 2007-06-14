package pl.tzr.driver.loxim;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Utility function used during communication with C++
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
class ConversionUtils {
	/**
	 * Writes C++ readable <i>unsigned int</i> value to the output stream 
	 * @param stream target output stream
	 * @param uint value to be written
	 * @throws IOException
	 */
	public static void writeUInt(
			OutputStream stream, 
			long uint)
		throws IOException {
		
		stream.write((int)(uint >> 24));
		stream.write((int)(uint >> 16));
		stream.write((int)(uint >> 8));
		stream.write((int)(uint % 256));
	}
	
	/**
	 * Writes C++ readable <i>char[]</i> value to the output stream 
	 * @param stream target output stream
	 * @param string value to be written
	 * @throws IOException
	 */
	public static void writeString(
				OutputStream stream, 
				String string) 
		throws IOException {
		
		stream.write(string.getBytes());
		stream.write(0);		
	}
	
	/**
	 * Reads <i>unsigned int</i> value written by C++ from the input stream
	 * @param stream source input stream
	 * @return readed unsigned int 
	 * @throws IOException
	 */
	public static long readUInt(InputStream stream) throws IOException {	
		byte b[] = new byte[4];		
		stream.read(b);
		long result = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
		return result;		
	}
	
	/**
	 * Writes enumeration value (<i>enum</i> C++ type) to output stream
	 * @param stream output stream
	 * @param index index of the enumeration item
	 * @throws IOException
	 */
	public static void writeEnum(
			OutputStream stream, 
			short index) 
		throws IOException {
		
		stream.write(index);
	}
	
	/**
	 * Reads enumeration value (<i>enum</i> C++ type) from the input stream
	 * @param stream input stream
	 * @return index of the enumeration item
	 * @throws IOException
	 */
	public static short readEnum(InputStream s) throws IOException{
		return (short)s.read();
	}	
	
	/**
	 * Reads double value (<i>double</i> C++ type) from the input stream
	 * @param stream input stream
	 * @return readed value
	 * @throws IOException
	 */
	public static double readDouble(InputStream stream) throws IOException {
		/* TODO */
		throw new UnsupportedOperationException("Not implemented yet");
	}
	
	/**
	 * Writes double value (<i>double</i> C++ type) to output stream
	 * @param stream output stream
	 * @param value value to be writted
	 * @throws IOException
	 */	
	public static void writeDouble(OutputStream stream, double value) {
		/* TODO */
		throw new UnsupportedOperationException("Not implemented yet");		
	}
	
	/**
	 * Returns storage size of the <i>enum</i> type
	 * @return storage size of the <i>enum</i> type
	 */
	public static int getEnumSize() {
		return 1;
	}
	
	/**
	 * Returns storage size of the <i>unsigned int</i> type
	 * @return storage size of the <i>unsigned int</i> type
	 */
	public static int getUIntSize() {
		return 4;
	}

	/**
	 * Returns storage size of the <i>double</i> type
	 * @return storage size of the <i>double</i> type
	 */	
	public static int getDoubleSize() {
		return 8;
	}
}
