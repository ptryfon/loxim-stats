package pl.edu.mimuw.loxim.jdbc;

import java.sql.SQLException;

public class LoXiMResultSetMetaDataImpl implements LoXiMResultSetMetaData {

	@Override
	public String getCatalogName(int column) throws SQLException {
		return "";
	}

	@Override
	public String getColumnClassName(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public int getColumnCount() throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public int getColumnDisplaySize(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public String getColumnLabel(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public String getColumnName(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public int getColumnType(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public String getColumnTypeName(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public int getPrecision(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public int getScale(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public String getSchemaName(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public String getTableName(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public boolean isAutoIncrement(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public boolean isCaseSensitive(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public boolean isCurrency(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public boolean isDefinitelyWritable(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public int isNullable(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public boolean isReadOnly(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public boolean isSearchable(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public boolean isSigned(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public boolean isWritable(int column) throws SQLException {
		throw getColumnMetaException();
	}

	@Override
	public boolean isWrapperFor(Class<?> iface) throws SQLException {
		return LoXiMResultSetMetaData.class.equals(iface);
	}

	@Override
	public <T> T unwrap(Class<T> iface) throws SQLException {
		if (isWrapperFor(iface)) {
			return (T) this;
		}
		throw new SQLException("Not a wrapper for " + iface);
	}

	private SBQLIncompatibilityException getColumnMetaException() {
		return new SBQLIncompatibilityException("SBQL does not support constant column meta information");
	}
	
}
