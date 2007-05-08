package pl.tzr.driver.loxim.result;

import java.util.LinkedList;
import java.util.List;


public class ResultStruct extends ResultCollection {
	List<Result> items;
	
	public ResultStruct() {
		items = new LinkedList<Result>();
	}

	public List<Result> getItems() {
		return items;
	}
	public String toString() {
		return "STRUCT";
	}
	
	public String getLongDescription() {
		boolean wasFirst = false;
		StringBuffer sb = new StringBuffer();
		sb.append("STRUCT(");
		for (Result r : items) {
			if (wasFirst) sb.append(";"); else wasFirst = true;
			sb.append(r.toString());
		}
		sb.append(")");
		return sb.toString();
	}	
	
	public int getType() {
		return ResultType.RES_STRUCT;
	}	
}
