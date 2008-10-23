package pl.edu.mimuw.loxim.protogen.lang.java.template.streams;

import java.io.IOException;
import java.io.OutputStream;
import java.math.BigInteger;
import java.nio.ByteBuffer;

/**
 * <ol>
 * <li>sint8 = byte</li>
 * <li>sint16 = short</li>
 * <li>sint32 = int</li>
 * <li>sint64 = long</li>
 * 
 * <li>uint8 = short</li>
 * <li>uint16 = int</li>
 * <li>uint32 = long</li>
 * <li>uint64 = Biginteger</li>
 * </ol>
 * 
 * 
 */

public class PackageOutputStreamWriter {
	public static final short VARUINT_NULL = 250;
	public static final short VARUINT_2B = 251;
	public static final short VARUINT_4B = 252;
	public static final short VARUINT_8B = 253;
	private static final int MAX_SSTRING_LENGTH = VARUINT_NULL-1;

	private final byte[] tmp_byte_array = new byte[128];
	private final ByteBuffer tmp_byte_buffer = ByteBuffer.wrap(tmp_byte_array);

	private OutputStream os;

	public PackageOutputStreamWriter(OutputStream outputStream) {
		os = outputStream;
	}

	public void writeInt8(byte s) throws IOException {
		os.write(s);
	}

	public void writeInt16(short s) throws IOException {
		tmp_byte_buffer.clear();
		tmp_byte_buffer.putShort(s);
		os.write(tmp_byte_array, 0, 2);
	}

	public void writeInt32(int s) throws IOException {
		tmp_byte_buffer.clear();
		tmp_byte_buffer.putInt(s);
		os.write(tmp_byte_array, 0, 4);
	}

	public void writeInt64(long s) throws IOException {
		tmp_byte_buffer.clear();
		tmp_byte_buffer.putLong(s);
		os.write(tmp_byte_array, 0, 8);
	}

	public void writeUint8(short s) throws IOException {
		os.write(s);
	}

	public void writeUint16(int s) throws IOException {
		tmp_byte_buffer.clear();
		tmp_byte_buffer.putInt(s);
		os.write(tmp_byte_array, 2, 2);
	}

	public void writeUint32(long s) throws IOException {
		tmp_byte_buffer.clear();
		tmp_byte_buffer.putLong(s);
		os.write(tmp_byte_array, 4, 4);
	}

	public void writeUint64(BigInteger s) throws IOException {
		/*
		 * Zapalamy bit 70, by wypchn±æ bit znaku do bajtu 0-rowego (9-tego od
		 * prawej) a nastêpnie zapisujemy pozosta³e 8 bajtów
		 */
		os.write(s.setBit(70).toByteArray(), 1, 8);
	}

	public void writeVaruint(Long s) throws IOException {
		if (s == null) {
			writeUint8(VARUINT_NULL);
			return;
		}
		if (s < 0)
			throw new IOException("Varuint does not support values<0:" + s);
		if (s < 250) {
			writeUint8(s.shortValue());
		} else if (s < 256 * 256) {
			writeUint8(VARUINT_2B);
			writeUint16(s.intValue());
		} else if (s < (256l) * (256l) * (256l) * (256l)) {
			writeUint8(VARUINT_4B);
			writeUint32(s.longValue());
		} else {
			writeUint8(VARUINT_8B);
			writeUint64(new BigInteger(s.toString()));
		}
	}

	public void writeBool(boolean b) throws IOException {
		writeInt8((byte) (b ? 1 : 0));
	}
	
	public void writeDouble(double d) throws IOException {
		tmp_byte_buffer.clear();
		tmp_byte_buffer.putDouble(d);
		os.write(tmp_byte_array, 0, 8);
	}

	public void writeString(String s) throws IOException {
		if (s == null) {
			writeVaruint(null);
		} else {
			byte[] b = s.getBytes("UTF-8");
			writeVaruint(new Long(b.length));
			os.write(b);
		}
	}

	public void writeSString(String s) throws IOException {
		if(s!=null && s.length()>MAX_SSTRING_LENGTH)
			throw new IOException("Cannot serialize too long 'Short string'. Expected <"+MAX_SSTRING_LENGTH+" got: "+s.length());
		writeString(s);
	}

	public void writeBytes(byte[] b) throws IOException {
		if(b==null)
		{
			writeVaruint(null);
		}else{
			writeVaruint(new Long(b.length));
			os.write(b);
		}
				
	}

}
