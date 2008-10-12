package pl.edu.mimuw.loxim.jdbc;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.sql.Array;
import java.sql.Blob;
import java.sql.CallableStatement;
import java.sql.Clob;
import java.sql.DatabaseMetaData;
import java.sql.NClob;
import java.sql.ResultSet;
import java.sql.SQLClientInfoException;
import java.sql.SQLException;
import java.sql.SQLFeatureNotSupportedException;
import java.sql.SQLInvalidAuthorizationSpecException;
import java.sql.SQLWarning;
import java.sql.SQLXML;
import java.sql.Savepoint;
import java.sql.Struct;
import java.util.Calendar;
import java.util.EnumSet;
import java.util.Locale;
import java.util.Map;
import java.util.Properties;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import pl.edu.mimuw.loxim.parser.Parser;
import pl.edu.mimuw.loxim.parser.SBQLParser;
import pl.edu.mimuw.loxim.protocol.enums.Auth_methodsEnum;
import pl.edu.mimuw.loxim.protocol.enums.Bye_reasonsEnum;
import pl.edu.mimuw.loxim.protocol.enums.CollationsEnum;
import pl.edu.mimuw.loxim.protocol.packages.A_sc_byePackage;
import pl.edu.mimuw.loxim.protocol.packages.A_sc_okPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_c_authorizedPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_c_helloPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_c_loginPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_c_passwordPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_s_helloPackage;
import pl.edu.mimuw.loxim.protogen.lang.java.template.auth.AuthException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.auth.AuthPassMySQL;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackageIO;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;

public class LoXiMConnectionImpl implements LoXiMConnection {

	private SQLWarning warning;
	
	private static final Log log = LogFactory.getLog(LoXiMConnectionImpl.class);

	private boolean autoCommit = true;
	private boolean closed = true;
	private PackageIO pacIO;
	private Parser parser = new SBQLParser();
	private int holdability = ResultSet.HOLD_CURSORS_OVER_COMMIT;

	public LoXiMConnectionImpl(ConnectionInfo info) throws IOException, ProtocolException,
			SQLInvalidAuthorizationSpecException, AuthException {

		log.debug("Creating connection with info:" + info);

		Socket socket = new Socket();
		socket.connect(new InetSocketAddress(info.getHost(), info.getPort()), info.getTimeout());
		boolean creationSuccess = false;

		try {
			pacIO = new PackageIO(socket);
			Package pac;

			// HELLO
			pac = new W_c_helloPackage(
					0L,
					InetAddress.getLocalHost().getHostName(), 
					LoXiMProperties.LoXiMDriverMajor + "." + LoXiMProperties.LoXiMDriverMinor, 
					InetAddress.getLocalHost().getHostName(), 
					Locale.getDefault().getLanguage(), 
					CollationsEnum.coll_default, 
					(byte) ((Calendar.getInstance().get(Calendar.ZONE_OFFSET) + Calendar.getInstance().get(Calendar.DST_OFFSET)) / (1000 * 60 * 60)));

			log.debug("Sending WCHello");
			pacIO.write(pac);
			log.debug("Receiving WSHello");
			pac = pacIO.read();
			if (pac.getPackageType() != W_s_helloPackage.ID) {
				throw new ProtocolException("Unexpected package. Expecting W_s_helloPackage");
			}
			W_s_helloPackage sHelloPackage = (W_s_helloPackage) pac;
			log.debug("Received WSHello");
			
			// LOGIN
			if (login(info.getUser(), info.getPassword(), sHelloPackage.getAuth_methods(), sHelloPackage.getSalt())) {
				log.debug("Client logged in");
			} else {
				log.debug("Client rejected");
				throw new SQLInvalidAuthorizationSpecException("No authorization");
			}
			creationSuccess = true;
		} finally {
			if (!creationSuccess) {
				socket.close();	
			}
		}
	}

	private boolean login(String login, String password, EnumSet<Auth_methodsEnum> authMethods, byte[] salt)
			throws ProtocolException, AuthException {
		log.info("Logging in");
		Auth_methodsEnum authMethod = selectAuthMethod(authMethods);
		log.debug("Authorization method selected: " + authMethod);
		log.debug("Sending login package");
		W_c_loginPackage cLoginPackage = new W_c_loginPackage(authMethod);
		pacIO.write(cLoginPackage);
		long pacType = pacIO.read().getPackageType();
		if (pacType != A_sc_okPackage.ID) {
			throw new ProtocolException("Server did not respond OK to login. Server responded with package #" + pacType);
		}

		log.debug("Sending authorization request");
		
		switch (authMethod) {
		case am_mysql5:
			sendAuth(login, new String(AuthPassMySQL.encodePassword(password, salt)));
			break;
		case am_trust:
			sendAuth(login, null);
			break;
		default:
			throw new ProtocolException("No authorization method provided");
		}

		log.debug("Reading authorization response");
		Package authResponse = pacIO.read();
		return authResponse.getPackageType() == W_c_authorizedPackage.ID; // FIXME W_*S*_AUTH...???
	}

	private Auth_methodsEnum selectAuthMethod(EnumSet<Auth_methodsEnum> authMethods) throws ProtocolException {
		log.debug("Authorization methods supported: " + authMethods);
		if (authMethods.contains(Auth_methodsEnum.am_mysql5)) {
			return Auth_methodsEnum.am_mysql5;
		}
		if (authMethods.contains(Auth_methodsEnum.am_trust)) {
			return Auth_methodsEnum.am_trust;
		}
		throw new ProtocolException("No authorization method provided");
	}

	private void sendAuth(String login, String password) throws ProtocolException {
		pacIO.write(new W_c_passwordPackage(login, password));
	}

	@Override
	public void clearWarnings() throws SQLException {
		checkClosed();
		warning = null;
	}

	@Override
	public void close() throws SQLException {
		if (closed) {
			return;
		}
		
		try {
			pacIO.write(new A_sc_byePackage(Bye_reasonsEnum.br_reason1, null));
			pacIO.close();
		} catch (ProtocolException e) {
			throw new SQLException(e);
		} catch (IOException e) {
			throw new SQLException(e);
		}
		closed = true;
	}

	@Override
	public void commit() throws SQLException {
		checkClosed();
		// TODO Auto-generated method stub

	}

	@Override
	public Array createArrayOf(String typeName, Object[] elements) throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Blob createBlob() throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Clob createClob() throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public NClob createNClob() throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public SQLXML createSQLXML() throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public LoXiMStatement createStatement() throws SQLException {
		return createStatement(ResultSet.TYPE_FORWARD_ONLY, ResultSet.CONCUR_READ_ONLY);
	}

	@Override
	public LoXiMStatement createStatement(int resultSetType, int resultSetConcurrency) throws SQLException {
		return createStatement(resultSetType, resultSetConcurrency, holdability);
	}

	@Override
	public LoXiMStatement createStatement(int resultSetType, int resultSetConcurrency, int resultSetHoldability)
			throws SQLException {
		checkClosed();
		overrideResultSetType(resultSetType);
		overrideResultSetConcurrency(resultSetConcurrency);
		overrideHoldability(resultSetHoldability);
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Struct createStruct(String typeName, Object[] attributes) throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public boolean getAutoCommit() throws SQLException {
		checkClosed();
		return autoCommit;
	}

	@Override
	public String getCatalog() throws SQLException {
		checkClosed();
		return null;
	}

	@Override
	public Properties getClientInfo() throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getClientInfo(String name) throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public int getHoldability() throws SQLException {
		checkClosed();
		return holdability;
	}

	@Override
	public LoXiMDatabaseMetaData getMetaData() throws SQLException {
		checkClosed();
		return new LoXiMDatabaseMetaDataImpl(this);
	}

	@Override
	public int getTransactionIsolation() throws SQLException {
		checkClosed();
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public Map<String, Class<?>> getTypeMap() throws SQLException {
		checkClosed();
		throw new SQLFeatureNotSupportedException("Type maps are not supported");
	}

	@Override
	public SQLWarning getWarnings() throws SQLException {
		return warning;
	}

	@Override
	public boolean isClosed() throws SQLException {
		return closed;
	}

	@Override
	public boolean isReadOnly() throws SQLException {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isValid(int timeout) throws SQLException {
		return isClosed(); // XXX
	}

	@Override
	public String nativeSQL(String sql) throws SQLException {
		return parser.parse(sql);
	}

	@Override
	public CallableStatement prepareCall(String sql) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public CallableStatement prepareCall(String sql, int resultSetType, int resultSetConcurrency) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public CallableStatement prepareCall(String sql, int resultSetType, int resultSetConcurrency,
			int resultSetHoldability) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public LoXiMPreparedStatement prepareStatement(String sql) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public LoXiMPreparedStatement prepareStatement(String sql, int autoGeneratedKeys) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public LoXiMPreparedStatement prepareStatement(String sql, int[] columnIndexes) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public LoXiMPreparedStatement prepareStatement(String sql, String[] columnNames) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public LoXiMPreparedStatement prepareStatement(String sql, int resultSetType, int resultSetConcurrency)
			throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public LoXiMPreparedStatement prepareStatement(String sql, int resultSetType, int resultSetConcurrency,
			int resultSetHoldability) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public void releaseSavepoint(Savepoint savepoint) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public void rollback() throws SQLException {
		checkClosed();
		// TODO Auto-generated method stub

	}

	@Override
	public void rollback(Savepoint savepoint) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public void setAutoCommit(boolean autoCommit) throws SQLException {
		checkClosed();
		this.autoCommit = autoCommit;
	}

	@Override
	public void setCatalog(String catalog) throws SQLException {
		checkClosed();
	}

	@Override
	public void setClientInfo(Properties properties) throws SQLClientInfoException {
		// TODO Auto-generated method stub

	}

	@Override
	public void setClientInfo(String name, String value) throws SQLClientInfoException {
		// TODO Auto-generated method stub

	}

	@Override
	public void setHoldability(int holdability) throws SQLException {
		checkClosed();
		overrideHoldability(holdability);
	}

	@Override
	public void setReadOnly(boolean readOnly) throws SQLException {
		checkClosed();
		// TODO Auto-generated method stub

	}

	@Override
	public Savepoint setSavepoint() throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public Savepoint setSavepoint(String name) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public void setTransactionIsolation(int level) throws SQLException {
		checkClosed();
		// TODO Auto-generated method stub

	}

	@Override
	public void setTypeMap(Map<String, Class<?>> map) throws SQLException {
		checkClosed();
		throw new SQLFeatureNotSupportedException("Type maps are not supported");
	}

	@Override
	public boolean isWrapperFor(Class<?> iface) throws SQLException {
		return LoXiMConnection.class.equals(iface);
	}

	@Override
	public <T> T unwrap(Class<T> iface) throws SQLException {
		if (isWrapperFor(iface)) {
			return (T) this;
		}
		throw new SQLException("Not a wrapper for " + iface);
	}

	@Override
	protected void finalize() throws SQLException {
		close();
	}
	
	private void checkClosed() throws SQLException {
		if (closed) {
			throw new SQLException("Connection is closed");
		}
	}
	
	private int overrideHoldability(int holdability) {
		switch (holdability) {
		case ResultSet.HOLD_CURSORS_OVER_COMMIT:
			return holdability;
		default:
			addWarning("Holdability: " + holdability + " is not supported. Using: " + ResultSet.HOLD_CURSORS_OVER_COMMIT + " (HOLD_CURSORS_OVER_COMMIT)");
			return ResultSet.HOLD_CURSORS_OVER_COMMIT;
		}
	}
	
	private int overrideResultSetType(int type) {
		switch (type) {
		case ResultSet.TYPE_FORWARD_ONLY:
			return type;
		default:
			addWarning("Result set type: " + type + " is not supported. Using: " + ResultSet.TYPE_FORWARD_ONLY + " (TYPE_FORWARD_ONLY)");
			return ResultSet.TYPE_FORWARD_ONLY;
		}
	}
	
	private int overrideResultSetConcurrency(int concurrency) {
		switch (concurrency) {
		case ResultSet.CONCUR_READ_ONLY:
			return concurrency;
		default:
			addWarning("Result set concurrency: " + concurrency + " is not supported. Using: " + ResultSet.CONCUR_READ_ONLY + " (CONCUR_READ_ONLY)");
			return ResultSet.CONCUR_READ_ONLY;
		}
		
		
	}
	
	private void addWarning(String msg) {
		SQLWarning warning = new SQLWarning(msg);
		if (this.warning == null) {
			this.warning = warning;
		} else {
			this.warning.setNextWarning(warning);
		}
	}
}
