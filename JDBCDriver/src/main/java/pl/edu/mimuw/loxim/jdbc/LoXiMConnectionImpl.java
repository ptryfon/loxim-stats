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
import java.util.Date;
import java.util.EnumSet;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Properties;
import java.util.SimpleTimeZone;
import java.util.TimeZone;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

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
import pl.edu.mimuw.loxim.protocol.packages.V_sc_sendvaluePackage;
import pl.edu.mimuw.loxim.protocol.packages.V_sc_sendvaluesPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_c_authorizedPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_c_helloPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_c_loginPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_c_passwordPackage;
import pl.edu.mimuw.loxim.protocol.packages.W_s_helloPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.BobPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.BoolPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.DatePackage;
import pl.edu.mimuw.loxim.protocol.packages_data.DatetimePackage;
import pl.edu.mimuw.loxim.protocol.packages_data.DatetimetzPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.DoublePackage;
import pl.edu.mimuw.loxim.protocol.packages_data.Sint16Package;
import pl.edu.mimuw.loxim.protocol.packages_data.Sint32Package;
import pl.edu.mimuw.loxim.protocol.packages_data.Sint64Package;
import pl.edu.mimuw.loxim.protocol.packages_data.Sint8Package;
import pl.edu.mimuw.loxim.protocol.packages_data.TimePackage;
import pl.edu.mimuw.loxim.protocol.packages_data.TimetzPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.Uint16Package;
import pl.edu.mimuw.loxim.protocol.packages_data.Uint32Package;
import pl.edu.mimuw.loxim.protocol.packages_data.Uint64Package;
import pl.edu.mimuw.loxim.protocol.packages_data.Uint8Package;
import pl.edu.mimuw.loxim.protocol.packages_data.VarcharPackage;
import pl.edu.mimuw.loxim.protogen.lang.java.template.auth.AuthException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.auth.AuthPassMySQL;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.BadPackageException;
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
	private Object statementMutex = new Object();

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
			W_s_helloPackage sHelloPackage = PackageUtil.readPackage(pacIO, W_s_helloPackage.class);
			log.debug("Received WSHello");
			
			// LOGIN
			if (login(info.getUser(), info.getPassword(), sHelloPackage.getAuth_methods(), sHelloPackage.getSalt())) {
				log.debug("Client logged in");
			} else {
				log.debug("Client rejected");
				throw new SQLInvalidAuthorizationSpecException("No authorization");
			}
			closed = false;
		} finally {
			if (closed) {
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
		PackageUtil.readPackage(pacIO, A_sc_okPackage.class);

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
		
		rollback0();
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
		if (autoCommit) {
			throw new SQLException("Cannot commit with autocommit mode on");
		}
		commit0();
	}

	private void commit0() {
		// TODO
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
		return autoCommit;
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
		// TODO Auto-generated method stub
		return 0;
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
		return isClosed(); // XXX
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
		if (autoCommit) {
			throw new SQLException("Cannot rollback with autocommit mode on");
		}
		rollback0();
	}

	private void rollback0() {
		// TODO
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
		close();
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
	
	public List<Object> executeQuery(String query) throws SQLException {
		
		class ResultReader {
		
			public List<Object> readValues() throws ProtocolException {
				List<Object> results = new ArrayList<Object>();

				for (Package pac = PackageUtil.readPackage(pacIO, Package.class); pac.getPackageType() == V_sc_sendvaluePackage.ID; pac = PackageUtil.readPackage(pacIO, Package.class)) {
					results.add(readValue(((V_sc_sendvaluePackage) pac).getData()));
				}
				
				return results;
			}
			
			private Object readValue(Package dataPac) throws ProtocolException {
				switch ((int) dataPac.getPackageType()) {
				
				// simple types
					// single
				case (int) Uint8Package.ID:
					return ((Uint8Package) dataPac).getValue();
				case (int) Uint16Package.ID:
					return ((Uint16Package) dataPac).getValue();
				case (int) Uint32Package.ID:
					return ((Uint32Package) dataPac).getValue();
				case (int) Uint64Package.ID:
					return ((Uint64Package) dataPac).getValue();
				case (int) Sint8Package.ID:
					return ((Uint8Package) dataPac).getValue();
				case (int) Sint16Package.ID:
					return ((Uint8Package) dataPac).getValue();
				case (int) Sint32Package.ID:
					return ((Uint8Package) dataPac).getValue();
				case (int) Sint64Package.ID:
					return ((Uint8Package) dataPac).getValue();
				case (int) BoolPackage.ID:
					return ((BoolPackage) dataPac).getValue();
				case (int) DatePackage.ID:
					DatePackage datePac = (DatePackage) dataPac;
					Calendar calD = Calendar.getInstance();
					calD.clear();
					calD.set(datePac.getYear(), datePac.getMonth(), datePac.getDay());
					return calD;
				case (int) TimePackage.ID:
					TimePackage timePac = (TimePackage) dataPac;
					Calendar calT = Calendar.getInstance();
					calT.clear();
					calT.set(Calendar.HOUR_OF_DAY, timePac.getHour());
					calT.set(Calendar.MINUTE, timePac.getMinuts());
					calT.set(Calendar.SECOND, timePac.getSec());
					calT.set(Calendar.MILLISECOND, timePac.getMilis());
					return calT;
				case (int) DatetimePackage.ID:
					DatetimePackage datetimePac = (DatetimePackage) dataPac;
					Calendar dtDate = (Calendar) readValue(datetimePac.getDate());
					Calendar dtTime = (Calendar) readValue(datetimePac.getTime());
					dtTime.set(dtDate.get(Calendar.YEAR), dtDate.get(Calendar.MONTH), dtDate.get(Calendar.DATE));
					return dtTime;
				case (int) TimetzPackage.ID:
					TimetzPackage timetzPac = (TimetzPackage) dataPac;
					Calendar timeCal = (Calendar) readValue(timetzPac.getTime());
					timeCal.setTimeZone(new SimpleTimeZone(timetzPac.getTz(), "DB-Timezone"));
					return timeCal;
				case (int) DatetimetzPackage.ID:
					DatetimetzPackage datetimetzPac = (DatetimetzPackage) dataPac;
					Calendar dCal = (Calendar) readValue(datetimetzPac.getDate());
					Calendar tCal = (Calendar) readValue(datetimetzPac.getTime());
					tCal.set(dCal.get(Calendar.YEAR), dCal.get(Calendar.MONTH), dCal.get(Calendar.DATE));
					tCal.setTimeZone(new SimpleTimeZone(datetimetzPac.getTz(), "DB-Timezone"));
					return tCal;
				case (int) DoublePackage.ID:
					return ((DoublePackage) dataPac).getValue();

					// multi
				case (int) BobPackage.ID:
				case (int) VarcharPackage.ID:


					return null;

				
				// complex types
					// single
				
					// multi
				default:
					throw new ProtocolException("Unhandled value type: " + dataPac.getPackageType());
				}
			}
			
		}
		
		synchronized (statementMutex) {
			try {
				Q_c_statementPackage statementPac = new Q_c_statementPackage();
				EnumSet<Statement_flagsEnum> flags = EnumSet.of(Statement_flagsEnum.sf_execute);
				statementPac.setFlags(flags);
				statementPac.setStatement(query);
				pacIO.write(statementPac);
				Package pac = pacIO.read();
				switch ((byte) pac.getPackageType()) {
				case A_sc_errorPackage.ID:
					throw new SQLException("Statement execution error: " + PackageUtil.toString((A_sc_errorPackage) pac));
				case Q_s_executingPackage.ID:
					PackageUtil.readPackage(pacIO, V_sc_sendvaluesPackage.class);
					ResultReader reader = new ResultReader();
					return reader.readValues();
				default:
					throw new BadPackageException(pac.getClass());
				}
			} catch (ProtocolException e) {
				throw new SQLException("Communication error", e);
			}
		}
	}
}
