package pl.edu.mimuw.loxim.protogen.lang.java.template.util;

import java.nio.ByteBuffer;

/**
 * http://darksleep.com/player/JavaAndUnsignedTypes.html
 * 
 * @author ptab
 */
public class ByteArrayUtil {
	
	public static void setOffsetUint8(byte[] b,int offset,short s)
	{
		b[offset]= (byte)(s & 0xFF);
	}
	
	public static void setOffsetUint16(byte[] b,int offset,int s)
	{
		byte[] tmp_byte_array = new byte[4];
		ByteBuffer tmp_byte_buffer = ByteBuffer.wrap(tmp_byte_array);
		
		tmp_byte_buffer.putInt(s);
		
		b[offset] = tmp_byte_array[2];
		b[offset+1] = tmp_byte_array[3];		
	}
	
	public static void setOffsetUint32(byte[] b,int offset,long s)
	{
		byte[] tmp_byte_array = new byte[8];
		ByteBuffer tmp_byte_buffer = ByteBuffer.wrap(tmp_byte_array);
		
		tmp_byte_buffer.putLong(s);
		
		b[offset] = tmp_byte_array[4];
		b[offset+1] = tmp_byte_array[5];		
		b[offset+2] = tmp_byte_array[6];
		b[offset+3] = tmp_byte_array[7];
	}
	
}
