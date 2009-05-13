package pl.edu.mimuw.loxim.jdbc;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;

public interface LoXiMResultSet extends ResultSet {

	public int[] findColumns(String columnLabel) throws SQLException;
	
	public List<Object> getObjects(String colName) throws SQLException;
	
}
