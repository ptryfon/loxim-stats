package pl.edu.mimuw.loxim.jdbc;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;

public interface LoXiMResultSet extends ResultSet {

	@Override
	public List<Object> getObject(int columnIndex) throws SQLException;
	
	@Override
	public List<Object> getObject(String columnLabel) throws SQLException;
	
	public int[] findColumns(String columnLabel) throws SQLException;
}
