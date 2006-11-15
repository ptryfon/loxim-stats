package loxim.driver;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.Map;

import loxim.driver.exception.SBQLException;
import loxim.driver.exception.SBQLIOException;
import loxim.driver.exception.SBQLProtocolException;
import loxim.driver.exception.SBQLServerException;
import loxim.driver.result.Result;
import loxim.driver.result.ResultError;


/**
 * Implementacja protokolu dostepu do bazy danych LoXiM za posrednictwem protokolu TCP/IP
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public class TcpConnection implements Connection {

	protected Socket socket;

	protected DataOutputStream dos;

	protected DataInputStream dis;

	/**
	 * Wykonuje zapytanie w oparciu o protokół sprzed wakacji 2006
	 * @param query
	 * @throws IOException
	 * @deprecated
	 */
	protected void oldSend(String query) throws IOException {
		dos.writeInt(query.length());
		dos.write(query.getBytes());
	}

	/**
	 * Wysyła pakiet na serwer bazy danych
	 * @param pack pakiet do wyslania
	 * @throws SBQLException
	 */
	protected void sendPackage(Package pack) throws SBQLException {
		try {
			ByteArrayOutputStream stream = new ByteArrayOutputStream();
			int size = pack.serialize(stream);
			ConversionUtils.writeUInt(dos, size);
			stream.writeTo(dos);
			assert(size == stream.size());
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}
	}


	/**
	 * Pobiera pakiet z serwera bazy danych
	 * @return pobrany pakiet
	 * @throws SBQLException
	 */
	protected Package readPackage() throws SBQLException {
		try {
			ResultStream rs = new ResultStream(dis);
			short type = ConversionUtils.readEnum(rs);
			Package pack = null;
			switch (type) {
				case Package.TYPE_SIMPLEQUERY:
					pack = new SimpleQueryPackage();
					break;
				case Package.TYPE_SIMPLERESULT:
					pack = new SimpleResultPackage();
					break;
				case Package.TYPE_PARAMQUERY:
					pack = new ParamQueryPackage();
					break;
				case Package.TYPE_PARAMSTATEMENT:
					pack = new ParamStatementPackage();
					break;
				case Package.TYPE_STATEMENT:
					pack = new StatementPackage();
					break;
				default:
					throw new SBQLProtocolException("Nieznany typ pakietu");
			}
			pack.deserialize(rs, (int)rs.length);
			rs.flush();
			return pack;
		} catch (IOException e) {
			throw new SBQLIOException(e);
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
		SimpleQueryPackage sqp = new SimpleQueryPackage(query);			
		sendPackage(sqp);
		
		Package p = readPackage();

		if (p instanceof SimpleResultPackage) {
			Result res = ((SimpleResultPackage)p).getResult();
			if (res instanceof ResultError)
				throw new SBQLServerException(((ResultError) res).getErrorCode());
			return res;				
		} else throw new SBQLProtocolException("Nieoczekiwany rodzaj pakietu " + p);			
	}

	public void close() throws SBQLException {
		try {
			socket.close();
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}
	}


	public long parse(String query) throws SBQLException {
		ParamQueryPackage sqp = new ParamQueryPackage(query);
		sendPackage(sqp);
		
		Package p = readPackage();
		
		if (p instanceof StatementPackage) {
			return ((StatementPackage)p).getStatementNumber();
		} else throw new SBQLProtocolException("Nieoczekiwany rodzaj pakietu " + p);			
	}
	
	public Result execute(long statementId, Map<String, Result> params) throws SBQLException {
		
		ParamStatementPackage psp = new ParamStatementPackage(statementId, params);
		sendPackage(psp);
		Package p = readPackage();

		if (p instanceof SimpleResultPackage) {
			Result res = ((SimpleResultPackage)p).getResult();
			if (res instanceof ResultError)
				throw new SBQLServerException(((ResultError) res).getErrorCode());
			return res;				
		} else throw new SBQLProtocolException("Nieoczekiwany rodzaj pakietu " + p);
			
	}
	

}
