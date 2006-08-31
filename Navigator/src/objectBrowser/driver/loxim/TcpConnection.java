package objectBrowser.driver.loxim;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

import objectBrowser.driver.loxim.result.*;

public class TcpConnection implements Connection {
	static final byte RES_RESULT = 0;

	static final byte RES_SEQUENCE = 1;

	static final byte RES_BAG = 2;

	static final byte RES_STRUCT = 3;

	static final byte RES_BINDER = 4;

	static final byte RES_STRING = 5;

	static final byte RES_INT = 6;

	static final byte RES_DOUBLE = 7;

	static final byte RES_BOOL = 8;

	static final byte RES_REFERENCE = 9;

	static final byte RES_VOID = 10;

	static final byte RES_ERROR = 11;

	static final byte RES_BOOLTRUE = 12;

	static final byte RES_BOOLFALSE = 13;

	Socket socket;

	DataOutputStream dos;

	DataInputStream dis;

	void oldSend(String query) throws IOException {
		dos.writeInt(query.length());
		dos.write(query.getBytes());
	}

	void packageSend(Package pack) throws IOException {
		ByteArrayOutputStream stream = new ByteArrayOutputStream();
		int size = pack.serialize(stream);
		ConversionUtils.writeUInt(dos, size);
		stream.writeTo(dos);
	}

	Result grabElements(ResultStream is, ResultCollection col)
			throws IOException, SBQLException {
		long count = is.readULong();
		for (int i = 0; i < count; i++) {
			col.addItem(deserialize(is));
		}
		return col;
	}

	String grabString(ResultStream is) throws IOException {
		String s = "";
		char c = (char) is.read();
		while (c != 0) {
			s = s + c;
			c = (char) is.read();
		}
		return s;
	}

	Result deserialize(ResultStream is) throws IOException, SBQLException {
		byte objType = (byte) is.read();
		switch (objType) {
		case RES_BAG:
			return grabElements(is, new ResultBag());
		case RES_SEQUENCE:
			return grabElements(is, new ResultSequence());
		case RES_STRUCT:
			return grabElements(is, new ResultStruct());
		case RES_REFERENCE:
			return new ResultReference(grabString(is));
		case RES_VOID:
			return new ResultVoid();
		case RES_STRING:
			return new ResultString(grabString(is));
		case RES_ERROR:
			return new ResultError(is.readULong());
		case RES_INT:
			return new ResultInt((int) is.readULong());
		case RES_BOOLTRUE:
			return new ResultBool(true);
		case RES_BOOLFALSE:
			return new ResultBool(false);
		case RES_DOUBLE:
			return null; /* TODO */
		case RES_BINDER:
			return new ResultBinder(grabString(is), deserialize(is));
		default:
			throw new SBQLException("Unknown result type (" + objType + ")",
					null);
		}
	}

	public TcpConnection(Socket _socket) throws IOException {
		socket = _socket;
		dis = new DataInputStream(socket.getInputStream());
		dos = new DataOutputStream(socket.getOutputStream());
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see szbd.client.Connection#execute(java.lang.String)
	 */
	public Result execute(String query) throws SBQLException {
		try {
			SimpleQueryPackage sqp = new SimpleQueryPackage();
			sqp.setQuery(query);

			packageSend(sqp);

			ResultStream rs = new ResultStream(dis);
			Result res = deserialize(rs);
			rs.flush();
			if (res instanceof ResultError)
				throw new SBQLException(((ResultError) res).getErrorCode());
			return res;
		} catch (IOException e) {
			throw new SBQLException(e.getMessage(), e);
		}
	}

	public Result getObjectByRef(String ref) throws SBQLException {
		return execute("deref(" + ref + ");");
	}

	public void close() throws SBQLException {
		try {
			socket.close();
		} catch (IOException e) {
			throw new SBQLException(e.getMessage(), e);
		}
	}
}
