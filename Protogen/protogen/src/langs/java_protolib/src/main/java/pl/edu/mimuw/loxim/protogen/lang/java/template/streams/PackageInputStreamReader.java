package pl.edu.mimuw.loxim.protogen.lang.java.template.streams;

import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.math.BigInteger;
import java.nio.ByteBuffer;

import pl.edu.mimuw.loxim.protogen.lang.java.template.util.StreamUtil;

public class PackageInputStreamReader {
	private InputStream is;

	public PackageInputStreamReader(InputStream inputStream) {
		is = inputStream;
	}

	// ============================ SIGNED ===================================
	public byte readInt8() throws IOException {
		int res = is.read();
		if (res < 0) {
			throw new EOFException();
		} else {
			return (byte) res;
		}
	}

	public short readInt16() throws IOException {
		return ByteBuffer.wrap(StreamUtil.readNbytes(is,2)).getShort();
	}

	public int readInt32() throws IOException {
		return ByteBuffer.wrap(StreamUtil.readNbytes(is,4)).getInt();
	}

	public long readInt64() throws IOException {
		return ByteBuffer.wrap(StreamUtil.readNbytes(is,8)).getLong();
	}

	// =========================== UNSIGNED ==================================

	public short readUint8() throws IOException {
		int res = is.read();
		if (res < 0) {
			throw new EOFException();
		} else {
			return (short) (0x000000FF & res);
		}
	}

	public int readUint16() throws IOException {
		byte[] b = new byte[4];
		StreamUtil.readNbytes(is,b,2,2);
		return ByteBuffer.wrap(b).getInt();
	}

	public long readUint32() throws IOException {
		byte[] b = new byte[8];
		StreamUtil.readNbytes(is,b,4,4);
		return ByteBuffer.wrap(b).getLong();
	}

	public BigInteger readUint64() throws IOException {
		byte[] b = StreamUtil.readNbytes(is,8);
		return new BigInteger(1,b);
	}

	// =======================================================================

	public boolean readBool() throws IOException {
		byte v = readInt8();
		switch (v) {
		case 1:
			return true;
		case 0:
			return false;
		default:
			throw new IOException("Unsupported boolean value: " + v);
		}
	}
	
	public Long readVaruint() throws IOException {
		short b0=readUint8();
		switch(b0){
			case PackageOutputStreamWriter.VARUINT_NULL:
				return null;
			case PackageOutputStreamWriter.VARUINT_2B:
				return new Long(readUint16());
			case PackageOutputStreamWriter.VARUINT_4B:
				return new Long(readUint32());
			case PackageOutputStreamWriter.VARUINT_8B:
				return new Long(readUint64().longValue());
			default: 
				return new Long(b0);
		}
	}

	public String readString() throws IOException{
		Long b0=readVaruint();
		if(b0==null)
			return null;
		byte[] b=StreamUtil.readNbytes(is,b0.intValue());
		return new String(b,"UTF-8");
	}
	
	public String readSString() throws IOException{
		short b0=readUint8();
		if(b0==PackageOutputStreamWriter.VARUINT_NULL)
			return null;
		if(b0>PackageOutputStreamWriter.VARUINT_NULL)
			throw new IOException("Trying to read short string, but the stream contains long string");
		byte[] b=StreamUtil.readNbytes(is,b0);
		return new String(b,"UTF-8");
	}
	
	public double readDouble() throws IOException {
		return ByteBuffer.wrap(StreamUtil.readNbytes(is,8)).getDouble();
	}
	
	public byte[] readNbytes(int n) throws IOException
	{
		return StreamUtil.readNbytes(is,n);
	}


	public byte[] readBytes() throws IOException {
		Long b0=readVaruint();
		if(b0==null)
			return null;
		return StreamUtil.readNbytes(is,b0.intValue());
	}

}
