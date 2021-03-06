package pl.edu.mimuw.loxim.jdbc;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.Reader;
import java.io.StringReader;
import java.math.BigDecimal;
import java.net.MalformedURLException;
import java.net.URL;
import java.sql.Array;
import java.sql.Blob;
import java.sql.Clob;
import java.sql.Date;
import java.sql.NClob;
import java.sql.Ref;
import java.sql.ResultSet;
import java.sql.RowId;
import java.sql.SQLDataException;
import java.sql.SQLException;
import java.sql.SQLFeatureNotSupportedException;
import java.sql.SQLWarning;
import java.sql.SQLXML;
import java.sql.Time;
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
import java.util.Map;

import pl.edu.mimuw.loxim.data.Binding;
import pl.edu.mimuw.loxim.data.LoXiMObject;
import pl.edu.mimuw.loxim.data.Struct;
import pl.edu.mimuw.loxim.jdbc.util.DateUtil;

public class LoXiMResultSetImpl implements LoXiMResultSet {

	private LoXiMStatement statement;
	private boolean closed;
	private SQLWarning warning;
	private int fetchSize;
	private boolean wasNull;
	private int index = -1;
	private List<Object> results;

	LoXiMResultSetImpl(LoXiMStatement statement, List<Object> results) throws SQLException {
		this.statement = statement;
		this.fetchSize = statement.getFetchSize();
		this.results = results;
	}

	@Override
	public boolean absolute(int row) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is TYPE_FORWARD_ONLY");
	}

	@Override
	public void afterLast() throws SQLException {
		checkClosed();
		throw new SQLException("The result set is TYPE_FORWARD_ONLY");
	}

	@Override
	public void beforeFirst() throws SQLException {
		checkClosed();
		throw new SQLException("The result set is TYPE_FORWARD_ONLY");
	}

	@Override
	public void cancelRowUpdates() throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
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

		closed = true;
	}

	@Override
	public void deleteRow() throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	private int findColumn0(String columnLabel) throws SQLException {
		checkClosed();
		int[] columns = findColumns(columnLabel);
		if (columns.length == 0) {
			return 0;
		}
		if (columns.length > 1) {
			throw new SQLDataException("Multiple columns (" + columns.length + ") for name " + columnLabel);
		}
		return columns[0];
	}

	@Override
	public int findColumn(String columnLabel) throws SQLException {
		int column = findColumn0(columnLabel);
		if (column == 0) {
			throw new SQLDataException("No such column: " + columnLabel);
		}
		return column;
	}

	@Override
	public int[] findColumns(String columnLabel) throws SQLException {
		checkClosed();
		Object o = results.get(index);
		if (o instanceof Struct) {
			List<Object> data = new ArrayList<Object>(((Struct) o).getData());
			List<Integer> cols = new ArrayList<Integer>(data.size());
			for (int i = 0; i < data.size(); i++) {
				Object object = data.get(i);
				if (object instanceof Binding) {
					Binding binding = (Binding) object;
					if (binding.getBindingName().equals(columnLabel)) {
						cols.add(i);
					}
				}
			}
			int[] colArray = new int[cols.size()];
			for (int i = 0; i < cols.size(); i++) {
				colArray[i] = cols.get(i) + 1;
			}
			return colArray;
		} else if (o instanceof Binding) {
			Binding binding = (Binding) o;
			if (binding.getBindingName().equals(columnLabel)) {
				return new int[] { 1 };
			} else {
				return new int[0];
			}
		} else {
			return new int[0];
		}
	}

	@Override
	public boolean first() throws SQLException {
		checkClosed();
		throw new SQLException("The result set is TYPE_FORWARD_ONLY");
	}

	@Override
	public Array getArray(int columnIndex) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public Array getArray(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getArray(col);
	}

	@Override
	public InputStream getAsciiStream(int columnIndex) throws SQLException {
		return getBinaryStream(columnIndex);
	}

	@Override
	public InputStream getAsciiStream(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getAsciiStream(col);
	}

	@Override
	public BigDecimal getBigDecimal(int columnIndex) throws SQLException {
		checkClosed();
		Object o = getColumn(columnIndex);
		if (o == null) {
			return null;
		}

		Number n = convertResult(columnIndex, Number.class);
		return BigDecimal.valueOf(n.doubleValue());
	}

	@Override
	public BigDecimal getBigDecimal(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getBigDecimal(col);
	}

	@Override
	public BigDecimal getBigDecimal(int columnIndex, int scale) throws SQLException {
		BigDecimal res = getBigDecimal(columnIndex);
		return res == null ? null : res.setScale(scale);
	}

	@Override
	public BigDecimal getBigDecimal(String columnLabel, int scale) throws SQLException {
		BigDecimal res = getBigDecimal(columnLabel);
		return res == null ? null : res.setScale(scale);
	}

	@Override
	public InputStream getBinaryStream(int columnIndex) throws SQLException {
		checkClosed();
		byte[] bytes = getBytes(columnIndex);
		if (bytes == null) {
			return null;
		}
		return new ByteArrayInputStream(bytes);
	}

	@Override
	public InputStream getBinaryStream(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getBinaryStream(col);
	}

	@Override
	public Blob getBlob(int columnIndex) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public Blob getBlob(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getBlob(col);
	}

	@Override
	public boolean getBoolean(int columnIndex) throws SQLException {
		checkClosed();
		Object o = convertResult(columnIndex, Object.class);
		if (o == null) {
			return false;
		}
		String boolStr = o.toString();
		if (boolStr.equals("1")) {
			return true;
		}
		if (boolStr.equals("0")) {
			return false;
		}
		throw new IllegalArgumentException("Object at index " + columnIndex + " cannot be mapped from " + o.getClass()
				+ " to boolean");
	}

	@Override
	public boolean getBoolean(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? false : getBoolean(col);
	}

	@Override
	public byte getByte(int columnIndex) throws SQLException {
		checkClosed();
		return convertResult(columnIndex, Number.class).byteValue();
	}

	@Override
	public byte getByte(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? 0 : getByte(col);
	}

	@Override
	public byte[] getBytes(int columnIndex) throws SQLException {
		checkClosed();
		Object o = getColumn(columnIndex);
		if (o == null) {
			return null;
		}

		if (o instanceof byte[]) {
			return (byte[]) o;
		}

		if (o instanceof String) {
			return ((String) o).getBytes();
		}

		throw new IllegalArgumentException("Object at index " + columnIndex + " cannot be mapped from " + o.getClass()
				+ " to byte[]");
	}

	@Override
	public byte[] getBytes(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getBytes(col);
	}

	@Override
	public Reader getCharacterStream(int columnIndex) throws SQLException {
		checkClosed();
		String s = convertResult(columnIndex, String.class);
		return s == null ? null : new StringReader(s);
	}

	@Override
	public Reader getCharacterStream(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getCharacterStream(col);
	}

	@Override
	public Clob getClob(int columnIndex) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public Clob getClob(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getClob(col);
	}

	@Override
	public int getConcurrency() throws SQLException {
		return ResultSet.CONCUR_READ_ONLY;
	}

	@Override
	public String getCursorName() throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public Date getDate(int columnIndex) throws SQLException {
		Calendar cal = convertResult(columnIndex, Calendar.class);
		return cal == null ? null : new Date(cal.getTimeInMillis());
	}

	@Override
	public Date getDate(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getDate(col);
	}

	@Override
	public Date getDate(int columnIndex, Calendar cal) throws SQLException {
		Date date = getDate(columnIndex);

		if (date == null || cal == null) {
			return date;
		}

		cal.setTime(date);
		DateUtil.resetToDate(cal);
		return new Date(cal.getTime().getTime());
	}

	@Override
	public Date getDate(String columnLabel, Calendar cal) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getDate(col, cal);
	}

	@Override
	public double getDouble(int columnIndex) throws SQLException {
		checkClosed();
		return convertResult(columnIndex, Number.class).doubleValue();
	}

	@Override
	public double getDouble(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? 0 : getDouble(col);
	}

	@Override
	public int getFetchDirection() throws SQLException {
		checkClosed();
		return ResultSet.FETCH_FORWARD;
	}

	@Override
	public int getFetchSize() throws SQLException {
		checkClosed();
		return fetchSize;
	}

	@Override
	public float getFloat(int columnIndex) throws SQLException {
		checkClosed();
		return convertResult(columnIndex, Number.class).floatValue();
	}

	@Override
	public float getFloat(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? 0 : getFloat(col);
	}

	@Override
	public int getHoldability() throws SQLException {
		checkClosed();
		return ResultSet.HOLD_CURSORS_OVER_COMMIT;
	}

	@Override
	public int getInt(int columnIndex) throws SQLException {
		checkClosed();
		return convertResult(columnIndex, Number.class).intValue();
	}

	@Override
	public int getInt(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? 0 : getInt(col);
	}

	@Override
	public long getLong(int columnIndex) throws SQLException {
		checkClosed();
		return convertResult(columnIndex, Number.class).longValue();
	}

	@Override
	public long getLong(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? 0 : getLong(col);
	}

	@Override
	public LoXiMResultSetMetaData getMetaData() throws SQLException {
		return new LoXiMResultSetMetaDataImpl();
	}

	@Override
	public Reader getNCharacterStream(int columnIndex) throws SQLException {
		return getCharacterStream(columnIndex);
	}

	@Override
	public Reader getNCharacterStream(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getNCharacterStream(col);
	}

	@Override
	public NClob getNClob(int columnIndex) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public NClob getNClob(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getNClob(col);
	}

	@Override
	public String getNString(int columnIndex) throws SQLException {
		return getString(columnIndex);
	}

	@Override
	public String getNString(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getNString(col);
	}

	@Override
	public Object getObject(int columnIndex) throws SQLException {
		checkClosed();
		return getColumn(columnIndex);
	}

	@Override
	public Object getObject(String columnLabel) throws SQLException {
		List<Object> list = getObjects(columnLabel);
		if (list.isEmpty()) {
			return null;
		}
		if (list.size() == 1) {
			return list.get(0);
		}
		return list;
	}

	@Override
	public List<Object> getObjects(String columnLabel) throws SQLException {
		int[] cols = findColumns(columnLabel);
		List<Object> list = new ArrayList<Object>(cols.length);
		for (int i : cols) {
			list.add(getObject(i));
		}
		return list;
	}

	@Override
	public Object getObject(int columnIndex, Map<String, Class<?>> map) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public Object getObject(String columnLabel, Map<String, Class<?>> map) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getObject(col, map);
	}

	@Override
	public Ref getRef(int columnIndex) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public Ref getRef(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getRef(col);
	}

	@Override
	public int getRow() throws SQLException {
		return index + 1;
	}

	@Override
	public RowId getRowId(int columnIndex) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public RowId getRowId(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getRowId(col);
	}

	@Override
	public SQLXML getSQLXML(int columnIndex) throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	@Override
	public SQLXML getSQLXML(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getSQLXML(col);
	}

	@Override
	public short getShort(int columnIndex) throws SQLException {
		checkClosed();
		return convertResult(columnIndex, Number.class).shortValue();
	}

	@Override
	public short getShort(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? 0 : getShort(col);
	}

	@Override
	public LoXiMStatement getStatement() throws SQLException {
		checkClosed();
		return statement;
	}

	@Override
	public String getString(int columnIndex) throws SQLException {
		checkClosed();
		return convertResult(columnIndex, String.class);
	}

	@Override
	public String getString(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getString(col);
	}

	@Override
	public Time getTime(int columnIndex) throws SQLException {
		Calendar cal = convertResult(columnIndex, Calendar.class);
		return cal == null ? null : new Time(cal.getTimeInMillis());
	}

	@Override
	public Time getTime(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getTime(col);
	}

	@Override
	public Time getTime(int columnIndex, Calendar cal) throws SQLException {
		Time t = getTime(columnIndex);

		if (t == null || cal == null) {
			return t;
		}

		cal.setTime(t);
		DateUtil.resetToTime(cal);

		return new Time(cal.getTime().getTime());
	}

	@Override
	public Time getTime(String columnLabel, Calendar cal) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getTime(col, cal);
	}

	@Override
	public Timestamp getTimestamp(int columnIndex) throws SQLException {
		Calendar cal = convertResult(columnIndex, Calendar.class);
		return cal == null ? null : new Timestamp(cal.getTimeInMillis());
	}

	@Override
	public Timestamp getTimestamp(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getTimestamp(col);
	}

	@Override
	public Timestamp getTimestamp(int columnIndex, Calendar cal) throws SQLException {
		Timestamp ts = getTimestamp(columnIndex);

		if (cal != null && ts != null) {
			ts.setTime(DateUtil.getTimeInMillis(ts, null, cal));
		}

		return ts;
	}

	@Override
	public Timestamp getTimestamp(String columnLabel, Calendar cal) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getTimestamp(col, cal);
	}

	@Override
	public int getType() throws SQLException {
		return ResultSet.TYPE_FORWARD_ONLY;
	}

	@Override
	public URL getURL(int columnIndex) throws SQLException {
		try {
			String s = convertResult(columnIndex, String.class);
			return s == null ? null : new URL(s);
		} catch (MalformedURLException e) {
			throw new SQLException(e);
		}

	}

	@Override
	public URL getURL(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getURL(col);
	}

	@Override
	public InputStream getUnicodeStream(int columnIndex) throws SQLException {
		return getBinaryStream(columnIndex);
	}

	@Override
	public InputStream getUnicodeStream(String columnLabel) throws SQLException {
		int col = findColumn0(columnLabel);
		return col == 0 ? null : getUnicodeStream(col);
	}

	@Override
	public SQLWarning getWarnings() throws SQLException {
		checkClosed();
		return warning;
	}

	@Override
	public void insertRow() throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public boolean isAfterLast() throws SQLException {
		return results.isEmpty() ? false : index == results.size();
	}

	@Override
	public boolean isBeforeFirst() throws SQLException {
		return results.isEmpty() ? false : index == -1;
	}

	@Override
	public boolean isClosed() throws SQLException {
		return closed;
	}

	@Override
	public boolean isFirst() throws SQLException {
		return index == 0;
	}

	@Override
	public boolean isLast() throws SQLException {
		return index == results.size() - 1;
	}

	@Override
	public boolean last() throws SQLException {
		checkClosed();
		throw new SQLException("The result set is TYPE_FORWARD_ONLY");
	}

	@Override
	public void moveToCurrentRow() throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void moveToInsertRow() throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public boolean next() throws SQLException {
		checkClosed();
		wasNull = false;
		if (index < results.size()) {
			index++;
		}
		return index < results.size();
	}

	@Override
	public boolean previous() throws SQLException {
		checkClosed();
		throw new SQLException("The result set is TYPE_FORWARD_ONLY");
	}

	@Override
	public void refreshRow() throws SQLException {
		checkClosed();
		throw new SQLException("The result set is TYPE_FORWARD_ONLY");
	}

	@Override
	public boolean relative(int rows) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is TYPE_FORWARD_ONLY");
	}

	@Override
	public boolean rowDeleted() throws SQLException {
		return false;
	}

	@Override
	public boolean rowInserted() throws SQLException {
		return false;
	}

	@Override
	public boolean rowUpdated() throws SQLException {
		return false;
	}

	@Override
	public void setFetchDirection(int direction) throws SQLException {
		checkClosed();
		switch (direction) {
		case ResultSet.FETCH_FORWARD:
			return;
		case ResultSet.FETCH_REVERSE:
		case ResultSet.FETCH_UNKNOWN:
			throw new SQLException("Bad fetch direction: " + direction + " - the result set is TYPE_FORWARD_ONLY");
		default:
			throw new SQLException("Unknown fetch direction: " + direction);
		}
	}

	@Override
	public void setFetchSize(int rows) throws SQLException {
		checkClosed();
		if (rows < 0) {
			throw new SQLException("Fetch size must be >= 0");
		}
		if (rows > 0) {
			fetchSize = rows;
		}
	}

	@Override
	public void updateArray(int columnIndex, Array x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateArray(String columnLabel, Array x) throws SQLException {
		updateArray(findColumn(columnLabel), x);
	}

	@Override
	public void updateAsciiStream(int columnIndex, InputStream x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateAsciiStream(String columnLabel, InputStream x) throws SQLException {
		updateAsciiStream(findColumn(columnLabel), x);
	}

	@Override
	public void updateAsciiStream(int columnIndex, InputStream x, int length) throws SQLException {
		updateAsciiStream(columnIndex, x, (long) length);
	}

	@Override
	public void updateAsciiStream(String columnLabel, InputStream x, int length) throws SQLException {
		updateAsciiStream(findColumn(columnLabel), x, length);
	}

	@Override
	public void updateAsciiStream(int columnIndex, InputStream x, long length) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateAsciiStream(String columnLabel, InputStream x, long length) throws SQLException {
		updateAsciiStream(findColumn(columnLabel), x, length);
	}

	@Override
	public void updateBigDecimal(int columnIndex, BigDecimal x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateBigDecimal(String columnLabel, BigDecimal x) throws SQLException {
		updateBigDecimal(findColumn(columnLabel), x);
	}

	@Override
	public void updateBinaryStream(int columnIndex, InputStream x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateBinaryStream(String columnLabel, InputStream x) throws SQLException {
		updateBinaryStream(findColumn(columnLabel), x);
	}

	@Override
	public void updateBinaryStream(int columnIndex, InputStream x, int length) throws SQLException {
		updateBinaryStream(columnIndex, x, (long) length);
	}

	@Override
	public void updateBinaryStream(String columnLabel, InputStream x, int length) throws SQLException {
		updateBinaryStream(findColumn(columnLabel), x, length);
	}

	@Override
	public void updateBinaryStream(int columnIndex, InputStream x, long length) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateBinaryStream(String columnLabel, InputStream x, long length) throws SQLException {
		updateBinaryStream(findColumn(columnLabel), x, length);
	}

	@Override
	public void updateBlob(int columnIndex, Blob x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateBlob(String columnLabel, Blob x) throws SQLException {
		updateBlob(findColumn(columnLabel), x);
	}

	@Override
	public void updateBlob(int columnIndex, InputStream inputStream) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateBlob(String columnLabel, InputStream inputStream) throws SQLException {
		updateBlob(findColumn(columnLabel), inputStream);
	}

	@Override
	public void updateBlob(int columnIndex, InputStream inputStream, long length) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateBlob(String columnLabel, InputStream inputStream, long length) throws SQLException {
		updateBlob(findColumn(columnLabel), inputStream, length);
	}

	@Override
	public void updateBoolean(int columnIndex, boolean x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateBoolean(String columnLabel, boolean x) throws SQLException {
		updateBoolean(findColumn(columnLabel), x);
	}

	@Override
	public void updateByte(int columnIndex, byte x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateByte(String columnLabel, byte x) throws SQLException {
		updateByte(findColumn(columnLabel), x);
	}

	@Override
	public void updateBytes(int columnIndex, byte[] x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateBytes(String columnLabel, byte[] x) throws SQLException {
		updateBytes(findColumn(columnLabel), x);
	}

	@Override
	public void updateCharacterStream(int columnIndex, Reader x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateCharacterStream(String columnLabel, Reader reader) throws SQLException {
		updateCharacterStream(findColumn(columnLabel), reader);
	}

	@Override
	public void updateCharacterStream(int columnIndex, Reader x, int length) throws SQLException {
		updateCharacterStream(columnIndex, x, (long) length);
	}

	@Override
	public void updateCharacterStream(String columnLabel, Reader reader, int length) throws SQLException {
		updateCharacterStream(findColumn(columnLabel), reader, length);
	}

	@Override
	public void updateCharacterStream(int columnIndex, Reader x, long length) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateCharacterStream(String columnLabel, Reader reader, long length) throws SQLException {
		updateCharacterStream(findColumn(columnLabel), reader, length);
	}

	@Override
	public void updateClob(int columnIndex, Clob x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateClob(String columnLabel, Clob x) throws SQLException {
		updateClob(findColumn(columnLabel), x);
	}

	@Override
	public void updateClob(int columnIndex, Reader reader) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateClob(String columnLabel, Reader reader) throws SQLException {
		updateClob(findColumn(columnLabel), reader);
	}

	@Override
	public void updateClob(int columnIndex, Reader reader, long length) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateClob(String columnLabel, Reader reader, long length) throws SQLException {
		updateClob(findColumn(columnLabel), reader, length);
	}

	@Override
	public void updateDate(int columnIndex, Date x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateDate(String columnLabel, Date x) throws SQLException {
		updateDate(findColumn(columnLabel), x);
	}

	@Override
	public void updateDouble(int columnIndex, double x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateDouble(String columnLabel, double x) throws SQLException {
		updateDouble(findColumn(columnLabel), x);
	}

	@Override
	public void updateFloat(int columnIndex, float x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateFloat(String columnLabel, float x) throws SQLException {
		updateFloat(findColumn(columnLabel), x);
	}

	@Override
	public void updateInt(int columnIndex, int x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateInt(String columnLabel, int x) throws SQLException {
		updateInt(findColumn(columnLabel), x);
	}

	@Override
	public void updateLong(int columnIndex, long x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateLong(String columnLabel, long x) throws SQLException {
		updateLong(findColumn(columnLabel), x);
	}

	@Override
	public void updateNCharacterStream(int columnIndex, Reader reader) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateNCharacterStream(String columnLabel, Reader reader) throws SQLException {
		updateNCharacterStream(findColumn(columnLabel), reader);
	}

	@Override
	public void updateNCharacterStream(int columnIndex, Reader reader, long length) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateNCharacterStream(String columnLabel, Reader reader, long length) throws SQLException {
		updateNCharacterStream(findColumn(columnLabel), reader, length);
	}

	@Override
	public void updateNClob(int columnIndex, NClob clob) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateNClob(String columnLabel, NClob clob) throws SQLException {
		updateNClob(findColumn(columnLabel), clob);
	}

	@Override
	public void updateNClob(int columnIndex, Reader reader) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateNClob(String columnLabel, Reader reader) throws SQLException {
		updateNClob(findColumn(columnLabel), reader);
	}

	@Override
	public void updateNClob(int columnIndex, Reader reader, long length) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateNClob(String columnLabel, Reader reader, long length) throws SQLException {
		updateNClob(findColumn(columnLabel), reader, length);
	}

	@Override
	public void updateNString(int columnIndex, String string) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateNString(String columnLabel, String string) throws SQLException {
		updateNString(findColumn(columnLabel), string);
	}

	@Override
	public void updateNull(int columnIndex) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateNull(String columnLabel) throws SQLException {
		updateNull(findColumn(columnLabel));
	}

	@Override
	public void updateObject(int columnIndex, Object x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateObject(String columnLabel, Object x) throws SQLException {
		updateObject(findColumn(columnLabel), x);
	}

	@Override
	public void updateObject(int columnIndex, Object x, int scaleOrLength) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateObject(String columnLabel, Object x, int scaleOrLength) throws SQLException {
		updateObject(findColumn(columnLabel), x, scaleOrLength);
	}

	@Override
	public void updateRef(int columnIndex, Ref x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateRef(String columnLabel, Ref x) throws SQLException {
		updateRef(findColumn(columnLabel), x);
	}

	@Override
	public void updateRow() throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateRowId(int columnIndex, RowId x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateRowId(String columnLabel, RowId x) throws SQLException {
		updateRowId(findColumn(columnLabel), x);
	}

	@Override
	public void updateSQLXML(int columnIndex, SQLXML xmlObject) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateSQLXML(String columnLabel, SQLXML xmlObject) throws SQLException {
		updateSQLXML(findColumn(columnLabel), xmlObject);
	}

	@Override
	public void updateShort(int columnIndex, short x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateShort(String columnLabel, short x) throws SQLException {
		updateShort(findColumn(columnLabel), x);
	}

	@Override
	public void updateString(int columnIndex, String x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateString(String columnLabel, String x) throws SQLException {
		updateString(findColumn(columnLabel), x);
	}

	@Override
	public void updateTime(int columnIndex, Time x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateTime(String columnLabel, Time x) throws SQLException {
		updateTime(findColumn(columnLabel), x);
	}

	@Override
	public void updateTimestamp(int columnIndex, Timestamp x) throws SQLException {
		checkClosed();
		throw new SQLException("The result set is CONCUR_READ_ONLY");
	}

	@Override
	public void updateTimestamp(String columnLabel, Timestamp x) throws SQLException {
		updateTimestamp(findColumn(columnLabel), x);
	}

	@Override
	public boolean wasNull() throws SQLException {
		checkClosed();
		return wasNull;
	}

	@Override
	public boolean isWrapperFor(Class<?> iface) throws SQLException {
		return LoXiMResultSet.class.equals(iface);
	}

	@Override
	public <T> T unwrap(Class<T> iface) throws SQLException {
		if (isWrapperFor(iface)) {
			return iface.cast(this);
		}
		throw new SQLException("Not a wrapper for " + iface);
	}

	private void checkClosed() throws SQLException {
		if (closed) {
			throw new SQLException("Result set is closed");
		}
	}

	/**
	 * Gets the object from the given column. First column is 1.
	 * 
	 * @param idx
	 * @return
	 * @throws SQLException
	 */
	private Object getColumn(int idx) throws SQLException {
		try {
			Object o = results.get(index);
			if (o instanceof Struct) {
				o = new ArrayList<Object>(((Struct) o).getData()).get(idx - 1);
				if (o instanceof Binding) {
					o = ((Binding) o).getValue();
				}
				wasNull = (o == null);
				return o;
			} else if (idx == 1) {
				if (o instanceof Binding) {
					o = ((Binding) o).getValue();
				}
				wasNull = (o == null);
				return o;
			} else {
				throw new IndexOutOfBoundsException();
			}
		} catch (IndexOutOfBoundsException e) {
			throw new SQLDataException("Column #" + idx + " does not exist", e);
		}
	}

	private <T> T convertResult(int idx, Class<T> clazz) throws SQLException, IllegalArgumentException {
		Object res = getColumn(idx);

		if (clazz.isInstance(res)) {
			return clazz.cast(res);
		}

		if (res == null) {
			if (Number.class.isAssignableFrom(clazz)) {
				return clazz.cast(0);
			}
			return null;
		}

		if (res instanceof LoXiMObject) {
			return ((LoXiMObject) res).mapTo(clazz);
		}

		throw new IllegalArgumentException("Object at index " + idx + " cannot be mapped from " + res.getClass()
				+ " to " + clazz);
	}

	@Override
	public String toString() {
		StringBuilder buf = new StringBuilder(results.size() * 16);
		buf.append("{\n");
		for (Object o : results) {
			buf.append(o);
			buf.append('\n');
		}
		buf.append("}");
		return buf.toString();
	}
}
