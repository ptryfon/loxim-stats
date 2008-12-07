package pl.edu.mimuw.loxim.jdbc;

import java.sql.ResultSet;
import java.sql.SQLException;

public interface LoXiMResultSet extends ResultSet {

	public int[] findColumns(String columnLabel) throws SQLException;
}
