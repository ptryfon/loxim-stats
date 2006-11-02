package objectBrowser.driver.loxim;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

import objectBrowser.driver.loxim.result.*;

public class TcpConnection implements Connection {

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
			
			SimpleResultPackage srp = new SimpleResultPackage();
			ResultStream rs = new ResultStream(dis);
			srp.deserialize(rs, (int)rs.length);
			Result res = srp.getResult();
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


	public Statement parse(String query) throws SBQLException {
		try {
			ParamQueryPackage sqp = new ParamQueryPackage();
			sqp.setQuery(query);
				
			packageSend(sqp);
			
			StatementPackage sp = new StatementPackage();
			ResultStream rs = new ResultStream(dis);
			sp.deserialize(rs, (int)rs.length);
			Statement stmt = new Statement(sp.getStatementNumber());
			rs.flush();
			
			return stmt;
		} catch (IOException e)  {
			throw new SBQLException(e.getMessage(), e);
		}
	}
	
	public Result execute(Statement statement) throws SBQLException {
		try {
			ParamStatementPackage psp = new ParamStatementPackage();
			psp.setStatementNumber(statement.getNumber());
			psp.setParams(statement.getParams());

			packageSend(psp);
			
			SimpleResultPackage srp = new SimpleResultPackage();
			ResultStream rs = new ResultStream(dis);
			srp.deserialize(rs, (int)rs.length);
			Result res = srp.getResult();
			rs.flush();
			
			if (res instanceof ResultError)
				throw new SBQLException(((ResultError) res).getErrorCode());
			return res;
		} catch (IOException e) {
			throw new SBQLException(e.getMessage(), e);
		}		
	}
	
}
