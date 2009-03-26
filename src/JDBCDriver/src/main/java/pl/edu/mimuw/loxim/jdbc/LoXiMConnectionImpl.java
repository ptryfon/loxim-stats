package pl.edu.mimuw.loxim.jdbc;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.sql.Array;
import java.sql.Blob;
import java.sql.CallableStatement;
import java.sql.Clob;
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
import java.util.ArrayList;
import java.util.Calendar;
import java.util.EnumSet;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Properties;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import pl.edu.mimuw.loxim.data.LoXiMCollection;
import pl.edu.mimuw.loxim.parser.Parser;
import pl.edu.mimuw.loxim.parser.SBQLParser;
import pl.edu.mimuw.loxim.protocol.enums.Auth_methodsEnum;
import pl.edu.mimuw.loxim.protocol.enums.Bye_reasonsEnum;
import pl.edu.mimuw.loxim.protocol.enums.CollationsEnum;
import pl.edu.mimuw.loxim.protocol.enums.Statement_flagsEnum;
import pl.edu.mimuw.loxim.protocol.packages.A_sc_byePackage;
import pl.edu.mimuw.loxim.protocol.packages.A_sc_errorPackage;
import pl.edu.mimuw.loxim.protocol.packages.A_sc_okPackage;
import pl.edu.mimuw.loxim.protocol.packages.PackageUtil;
import pl.edu.mimuw.loxim.protocol.packages.Q_c_statementPackage;
import pl.edu.mimuw.loxim.protocol.packages.Q_s_executingPackage;
import pl.edu.mimuw.loxim.protocol.packages.V_sc_abortPackage;
import pl.edu.mimuw.loxim.protocol.packages.V_sc_sendvaluesPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_c_authorizedPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_c_helloPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_c_loginPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_c_passwordPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_s_helloPackage;
import pl.edu.mimuw.loxim.protogen.lang.java.template.auth.AuthException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.auth.AuthPassMySQL;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.BadPackageException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackageIO;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;

public class LoXiMConnectionImpl implements LoXiMConnectionInternal {

	private SQLWarning warning;
	
	private static final Log log = LogFactory.getLog(LoXiMConnectionImpl.class);
	
	private static final String SQL_TR_COMMIT = "end";
	private static final String SQL_TR_ROLLBACK = "abort";
	private static final String SQL_TR_START = "begin";

	private volatile boolean closed = true;
	private PackageIO pacIO;
	private Parser parser = new SBQLParser();
	private final int holdability = ResultSet.HOLD_CURSORS_OVER_COMMIT;
	private final Object statementMutex = new Object();
	private boolean tx;
	
	public LoXiMConnectionImpl(ConnectionInfo info) throws IOException, ProtocolException,
			SQLInvalidAuthorizationSpecException, AuthException {

		log.debug("Creating connection with info:" + info);

		Socket socket = new Socket();
		socket.connect(new InetSocketAddress(info.getHost(), info.getPort()), info.getTimeout());

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
			W_s_helloPackage sHelloPackage = pacIO.read(W_s_helloPackage.class);
			log.debug("Received WSHello");
			
			// LOGIN
			if (login(info.getUser(), info.getPassword(), sHelloPackage.getAuth_methods(), sHelloPackage.getSalt())) {
				log.debug("Client logged in");
			} else {
				log.debug("Client rejected");
				throw new SQLInvalidAuthorizationSpecException("No authorization");
			}
			closed = false;
			log.info("Connection stablished successfully ");
		} finally {
			if (closed) { // ie. if an exception was thrown
				socket.close();
			}
		}
	}

	private boolean login(String login, String password, EnumSet<Auth_methodsEnum> authMethods, byte[] salt)
			throws ProtocolException, AuthException {
		log.debug("Logging in");
		Auth_methodsEnum authMethod = selectAuthMethod(authMethods);
		log.debug("Authorization method selected: " + authMethod);
		log.debug("Sending login package");
		W_c_loginPackage cLoginPackage = new W_c_loginPackage(authMethod);
		pacIO.write(cLoginPackage);
		pacIO.read(A_sc_okPackage.class);

		log.debug("Sending authorization request in mode " + authMethod);
		
		switch (authMethod) {
		case am_mysql5:
			sendAuth(login, new String(AuthPassMySQL.encodePassword(password, salt)));
			break;
		case am_trust:
			sendAuth(login, login); // XXX empty password would be more appropriate
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
		
		log.debug("Closing connection");
		
		try {
			pacIO.write(new A_sc_byePackage(Bye_reasonsEnum.br_reason1, null));
			pacIO.close();
		} catch (ProtocolException e) {
			throw new SQLException(e);
		} catch (IOException e) {
			throw new SQLException(e);
		} finally {
			closed = true;
		}
	}

	@Override
	public void commit() throws SQLException {
		checkClosed();
		tx_commit();
	}
	
	@Override
	public Array createArrayOf(String typeName, Object[] elements) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public Blob createBlob() throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public Clob createClob() throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public NClob createNClob() throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public SQLXML createSQLXML() throws SQLException {
		throw new SQLFeatureNotSupportedException();
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
		checkResultSetType(resultSetType);
		checkResultSetConcurrency(resultSetConcurrency);
		checkHoldability(resultSetHoldability);
		LoXiMStatement stmt = new LoXiMStatementImpl(this);
		return stmt;
	}

	@Override
	public Struct createStruct(String typeName, Object[] attributes) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public boolean getAutoCommit() throws SQLException {
		checkClosed();
		return false;
	}

	@Override
	public String getCatalog() throws SQLException {
		checkClosed();
		return null;
	}

	@Override
	public Properties getClientInfo() throws SQLException {
		checkClosed();
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getClientInfo(String name) throws SQLException {
		checkClosed();
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
		return 0; //TODO
	}

	@Override
	public Map<String, Class<?>> getTypeMap() throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public SQLWarning getWarnings() throws SQLException {
		checkClosed();
		return warning;
	}

	@Override
	public boolean isClosed() throws SQLException {
		return closed;
	}

	@Override
	public boolean isReadOnly() throws SQLException {
		checkClosed();
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isValid(int timeout) throws SQLException {
		if (timeout < 0) {
			throw new SQLException("Timeout cannot be less than 0");
		}
		return !isClosed(); // XXX
	}

	@Override
	public String nativeSQL(String sql) throws SQLException {
		checkClosed();
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
		tx_rollback();
	}
	
	private void tx_commit() throws SQLException {
		log.debug("Commit transaction");
		execute(createStatement(), SQL_TR_COMMIT);
		synchronized (statementMutex) {
			tx = false;
		}
	}
	
	private void tx_rollback() throws SQLException {
		log.debug("Rollback transaction");
		execute(createStatement(), SQL_TR_ROLLBACK);
		synchronized (statementMutex) {
			tx = false;
		}
	}
	
	private void tx_begin() throws SQLException {
		log.debug("Start transaction");
		synchronized (statementMutex) {
			tx = true;
		}
		execute(createStatement(), SQL_TR_START);
	}
	
	@Override
	public void rollback(Savepoint savepoint) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public void setAutoCommit(boolean autoCommit) throws SQLException {
		checkClosed();
		throw new SQLFeatureNotSupportedException();
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
		checkHoldability(holdability);
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
		throw new SQLFeatureNotSupportedException();
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
		if (!closed) {
			log.warn("Closing connection on finalization.");
			close();
		}
	}
	
	private void checkClosed() throws SQLException {
		if (closed) {
			throw new SQLException("Connection is closed");
		}
	}
	
	private void checkHoldability(int holdability) throws SQLFeatureNotSupportedException, SQLException {
		switch (holdability) {
		case ResultSet.HOLD_CURSORS_OVER_COMMIT:
			return;
		case ResultSet.CLOSE_CURSORS_AT_COMMIT:
			throw new SQLFeatureNotSupportedException("Holdability type: " + holdability + " is not supported");
		default:
			throw new SQLException("Not a holdability type");
		}
	}
	
	private void checkResultSetType(int type) throws SQLFeatureNotSupportedException, SQLException {
		switch (type) {
		case ResultSet.TYPE_FORWARD_ONLY:
			return;
		case ResultSet.TYPE_SCROLL_INSENSITIVE:
		case ResultSet.TYPE_SCROLL_SENSITIVE:
			throw new SQLFeatureNotSupportedException("Result set type: " + type + " is not supported");
		default:
			throw new SQLException("Not a result set type");
		}
	}
	
	private void checkResultSetConcurrency(int concurrency) throws SQLFeatureNotSupportedException, SQLException {
		switch (concurrency) {
		case ResultSet.CONCUR_READ_ONLY:
			return;
		case ResultSet.CONCUR_UPDATABLE:
			throw new SQLFeatureNotSupportedException("Result set concurrency: " + concurrency + " is not supported");
		default:
			throw new SQLException("Not a result set concurrency type");
		}
	}
	
	@Override
	public ExecutionResult execute(LoXiMStatement stmt, String sbql) throws SQLException {
		checkClosed();
		synchronized (statementMutex) {
			try {
				if (!tx) {
					log.debug("Starting transaction implicitly");
					tx_begin();
				}
				log.debug("Executing statement: '" + sbql + "'");
				Q_c_statementPackage statementPac = new Q_c_statementPackage();
				EnumSet<Statement_flagsEnum> flags = EnumSet.of(Statement_flagsEnum.sf_execute);
				statementPac.setFlags(flags);
				statementPac.setStatement(sbql);
				log.debug("Sending statement for execution");
				pacIO.write(statementPac);
				log.debug("Reading statement result of '" + sbql + "'");
				Package pac = pacIO.read();
				switch ((byte) pac.getPackageType()) {
				case A_sc_errorPackage.ID:
					throw new SQLException("Statement execution error: " + PackageUtil.toString((A_sc_errorPackage) pac));
				case Q_s_executingPackage.ID:
					pacIO.read(V_sc_sendvaluesPackage.class);
				case V_sc_sendvaluesPackage.ID: // FIXME bug workaround fallthrough 
					ResultReader reader = new ResultReader(pacIO);
					List<Object> res = unfoldResultList(reader.readValues());
//	FIXME bug workaround				Q_s_execution_finishedPackage exFin = PackageUtil.readPackage(pacIO, Q_s_execution_finishedPackage.class);
//					int updates = ((int) (exFin.getDelCnt() + exFin.getInsertsCnt() + exFin.getModAtomPointerCnt() + exFin.getNewRootsCnt()));
					return new ExecutionResult(stmt, res, 0); // FIXME 
				default:
					throw new BadPackageException(pac.getClass());
				}
			} catch (ProtocolException e) {
				throw new SQLException("Query execution error - " + e.getLocalizedMessage(), e);
			}
		}
	}
	
	private List<Object> unfoldResultList(List<Object> resultList) {
		List<Object> unfoldedResult;
		
		if (resultList.size() == 1) {
			Object res = resultList.get(0);
			if (res instanceof LoXiMCollection) {
				LoXiMCollection collection = (LoXiMCollection) res;
				unfoldedResult = new ArrayList<Object>(collection.getData());
			} else {
				unfoldedResult = resultList; 
			}
		} else {
			unfoldedResult = resultList;
		}
		return unfoldedResult;
	}
	
	@Override
	public void cancel() throws SQLException {
		checkClosed();
		try {
			V_sc_abortPackage pac = new V_sc_abortPackage();
			pacIO.write(pac);
		} catch (ProtocolException e) {
			throw new SQLException("Error when cancelling query execution", e);
		}
	}
}
