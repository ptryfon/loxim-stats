package pl.edu.mimuw.loxim.protogen.lang.java.template.streams;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.EOFException;
import java.io.IOException;
import java.math.BigInteger;

import org.junit.Test;

import static org.junit.Assert.*;

public class TestStreams {

	@Test
	public void testWriteBool() throws Exception {
		// Zapisy
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeBool(true);
		pos.writeBool(false);

		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		assertEquals(true, pis.readBool());
		assertEquals(false, pis.readBool());

		try {
			pis.readInt8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");
	}

	// ==================== SIGNED ======================================

	@Test
	public void testWriteInt8() throws Exception {
		// Zapisy
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeInt8((byte) 0);
		pos.writeInt8((byte) 10);
		pos.writeInt8((byte) -128);
		pos.writeInt8((byte) 127);

		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		assertEquals((byte) 0, pis.readInt8());
		assertEquals((byte) 10, pis.readInt8());
		assertEquals((byte) -128, pis.readInt8());
		assertEquals((byte) 127, pis.readInt8());

		try {
			pis.readInt8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");
	}

	@Test
	public void testWriteInt16() throws Exception {
		// Zapisy
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeInt16((short) 0);
		pos.writeInt16((short) -1111);
		pos.writeInt16((short) 1111);
		pos.writeInt16(Short.MIN_VALUE);
		pos.writeInt16(Short.MAX_VALUE);

		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		assertEquals((short) 0, pis.readInt16());
		assertEquals((short) -1111, pis.readInt16());
		assertEquals((short) 1111, pis.readInt16());
		assertEquals(Short.MIN_VALUE, pis.readInt16());
		assertEquals(Short.MAX_VALUE, pis.readInt16());

		try {
			pis.readInt8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");
	}

	@Test
	public void testWriteInt32() throws Exception {
		// Zapisy
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeInt32((int) 0);
		pos.writeInt32((int) -1111111);
		pos.writeInt32((int) 2222222);
		pos.writeInt32(Integer.MIN_VALUE);
		pos.writeInt32(Integer.MAX_VALUE);

		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		assertEquals((int) 0, pis.readInt32());
		assertEquals((int) -1111111, pis.readInt32());
		assertEquals((int) 2222222, pis.readInt32());
		assertEquals(Integer.MIN_VALUE, pis.readInt32());
		assertEquals(Integer.MAX_VALUE, pis.readInt32());

		try {
			pis.readInt8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");
	}

	@Test
	public void testWriteInt64() throws Exception {
		// Zapisy
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeInt64((long) 0);
		pos.writeInt64((long) -11111111234l);
		pos.writeInt64((long) 22222221234l);
		pos.writeInt64(Long.MIN_VALUE);
		pos.writeInt64(Long.MAX_VALUE);

		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		assertEquals((long) 0, pis.readInt64());
		assertEquals((long) -11111111234l, pis.readInt64());
		assertEquals((long) 22222221234l, pis.readInt64());
		assertEquals(Long.MIN_VALUE, pis.readInt64());
		assertEquals(Long.MAX_VALUE, pis.readInt64());

		try {
			pis.readInt8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");
	}

	// ==================== UNSIGNED ======================================

	@Test
	public void testWriteUint8() throws Exception {
		// Zapisy
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeUint8((short) 0);
		pos.writeUint8((short) 10);
		pos.writeUint8((short) 127);
		pos.writeUint8((short) 255);

		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		assertEquals((short) 0, pis.readUint8());
		assertEquals((short) 10, pis.readUint8());
		assertEquals((short) 127, pis.readUint8());
		assertEquals((short) 255, pis.readUint8());

		try {
			pis.readUint8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");
	}

	@Test
	public void testWriteUint16() throws Exception {
		// Zapisy
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeUint16((int) 11123);
		pos.writeUint16((int) 0);
		pos.writeUint16((int) 41123);
		pos.writeUint16(Short.MAX_VALUE);
		pos.writeUint16((int) (256 * 256 - 1));

		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		assertEquals((int) 11123, pis.readUint16());
		assertEquals((int) 0, pis.readUint16());
		assertEquals((int) 41123, pis.readUint16());
		assertEquals(Short.MAX_VALUE, pis.readUint16());
		assertEquals((int) (256 * 256 - 1), pis.readUint16());

		try {
			pis.readUint8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");
	}

	@Test
	public void testWriteUint32() throws Exception {
		// Zapisy
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeUint32((long) 0);
		pos.writeUint32((long) 2222222);
		pos.writeUint32(Integer.MAX_VALUE);
		pos.writeUint32(256l * 256l * 256l * 256l - 1);

		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		assertEquals((long) 0, pis.readUint32());
		assertEquals((long) 2222222, pis.readUint32());
		assertEquals(Integer.MAX_VALUE, pis.readUint32());
		assertEquals(256l * 256l * 256l * 256l - 1, pis.readUint32());

		try {
			pis.readUint8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");
	}

	@Test
	public void testWriteUint64() throws Exception {
		// Zapisy
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeUint64(new BigInteger("0"));
		pos.writeUint64(new BigInteger("1234567891234"));
		pos.writeUint64(new BigInteger("1").shiftLeft(60));
		pos.writeUint64(new BigInteger("1").shiftLeft(61));
		pos.writeUint64(new BigInteger("1").shiftLeft(62));
		pos.writeUint64(new BigInteger("1").shiftLeft(63));
		pos.writeUint64(new BigInteger("1").shiftLeft(64));
		pos.writeUint64(new BigInteger("987654321"));
		pos.writeUint64(new BigInteger("0"));

		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		assertEquals(new BigInteger("0"), pis.readUint64());
		assertEquals(new BigInteger("1234567891234"), pis.readUint64());
		assertEquals(new BigInteger("1").shiftLeft(60), pis.readUint64());
		assertEquals(new BigInteger("1").shiftLeft(61), pis.readUint64());
		assertEquals(new BigInteger("1").shiftLeft(62), pis.readUint64());
		assertEquals(new BigInteger("1").shiftLeft(63), pis.readUint64());
		assertTrue("Oczekiwano, ¿e obiekty bêd± ró¿ne", (BigInteger.ONE
				.shiftLeft(64) != pis.readUint64()));
		assertEquals(new BigInteger("987654321"), pis.readUint64());
		assertEquals(new BigInteger("0"), pis.readUint64());

		try {
			pis.readUint8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");
	}
	
	@Test
	public void testWriteVaruint() throws Exception {
		// Zapisy
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeVaruint(null);
		pos.writeVaruint((long) 0);
		pos.writeVaruint((long) 17);
		pos.writeVaruint((long) 249);
		pos.writeVaruint((long) 250);
		pos.writeVaruint((long) 255);
		pos.writeVaruint((long) 256);
		pos.writeVaruint((long) 2222);
		pos.writeVaruint((long) 22222);
		pos.writeVaruint((long) 222222);
		pos.writeVaruint((long) 2222222);
		pos.writeVaruint((long) 22222222);
		pos.writeVaruint((long) 222222222);
		pos.writeVaruint((long) 2222222222l);
		pos.writeVaruint((long) 22222222222l);
		pos.writeVaruint(256l * 256l * 256l * 256l - 1);
		pos.writeVaruint(Long.MAX_VALUE);
		

		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		assertEquals(null, pis.readVaruint());
		assertEquals(new Long(0), pis.readVaruint());
		assertEquals(new Long(17), pis.readVaruint());
		assertEquals(new Long(249), pis.readVaruint());
		assertEquals(new Long(250), pis.readVaruint());
		assertEquals(new Long(255), pis.readVaruint());
		assertEquals(new Long(256), pis.readVaruint());
		assertEquals(new Long(2222), pis.readVaruint());
		assertEquals(new Long(22222), pis.readVaruint());
		assertEquals(new Long(222222), pis.readVaruint());
		assertEquals(new Long(2222222), pis.readVaruint());
		assertEquals(new Long(22222222), pis.readVaruint());
		assertEquals(new Long(222222222), pis.readVaruint());
		assertEquals(new Long(2222222222l), pis.readVaruint());
		assertEquals(new Long(22222222222l), pis.readVaruint());
		assertEquals(new Long(256l * 256l * 256l * 256l - 1), pis.readVaruint());
		assertEquals(new Long(Long.MAX_VALUE), pis.readVaruint());

		try {
			pis.readUint8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");
	}
	
	@Test
	public void testWriteString() throws Exception
	{
		// Zapisy
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeString(null);
		pos.writeString("");
		pos.writeString("123");
		pos.writeString(times(100,"0123456789"));
		pos.writeString(times(100,"ê±¶æ¿¶ê"));

		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		pos.writeString(null);
		pos.writeString("");
		pos.writeString("123");
		pos.writeString(times(100,"0123456789"));
		pos.writeString(times(100,"ê±¶æ¿¶ê"));

		assertEquals(null, pis.readString());
		assertEquals("", pis.readString());
		assertEquals("123", pis.readString());
		assertEquals(times(100,"0123456789"), pis.readString());
		assertEquals(times(100,"ê±¶æ¿¶ê"), pis.readString());

		try {
			pis.readUint8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");		
	}
	
	@Test
	public void testWriteSString() throws Exception
	{
		boolean ok=false;
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeSString(null);
		pos.writeSString("");
		pos.writeSString("123");
		pos.writeSString(times(249,"a"));
		
		try{
			pos.writeSString(times(250,"b"));
		}catch (IOException e) {
			ok=true;
			return;
		}
		assertTrue("Expected exception",ok);
		ok=false;
		
		try{
			pos.writeSString(times(100,"0123456789"));
		}catch (IOException e) {
			ok=true;
			return;
		}
		assertTrue("Expected exception",ok);
		ok=false;
		
		try{
			pos.writeSString(times(100,"ê±¶æ¿¶ê"));
		}catch (IOException e) {
			ok=true;
			return;
		}
		assertTrue("Expected exception",ok);
		ok=false;

		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		assertEquals(null, pis.readSString());
		assertEquals("", pis.readSString());
		assertEquals("123", pis.readSString());
		assertEquals(times(249,"a"),pis.readSString());

		try {
			pis.readUint8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");		
	}
	
	@Test
	public void testWriteBytes() throws Exception
	{
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeBytes(null);
		pos.writeBytes("".getBytes());
		pos.writeBytes("123".getBytes());
		pos.writeBytes(times(249,"a").getBytes());
		
		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		assertArrayEquals(null, pis.readBytes());
		assertArrayEquals("".getBytes(), pis.readBytes());
		assertArrayEquals("123".getBytes(), pis.readBytes());
		assertArrayEquals(times(249,"a").getBytes(),pis.readBytes());

		try {
			pis.readUint8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");		
	}
	
	@Test
	public void testWriteDouble() throws Exception {
		// Zapisy
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		PackageOutputStreamWriter pos = new PackageOutputStreamWriter(os);

		pos.writeDouble(0.0);
		pos.writeDouble(-12345.6789);
		pos.writeDouble(9876543.123);

		os.close();
		ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
		PackageInputStreamReader pis = new PackageInputStreamReader(is);

		assertEquals(0.0, pis.readDouble(),0.0000001);
		assertEquals(-12345.6789, pis.readDouble(),0.0000001);
		assertEquals(9876543.123, pis.readDouble(),0.0000001);

		try {
			pis.readInt8();
		} catch (EOFException e) {
			return;
		} finally {
			is.close();
		}
		throw new Exception("Exception EOFException expected");
	}

	private String times(int n, String string) {
		StringBuffer s=new StringBuffer();
		for(int i=0; i<n; i++)
			s.append(string);
		return s.toString();
	}
}
