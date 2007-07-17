package pl.tzr.driver.loxim.result;

import java.util.LinkedList;
import java.util.List;


public class ResultSequence extends ResultCollection {
	List<Result> items;
	
	public ResultSequence() {
		items = new LinkedList<Result>();
	}

    @Override
	public List<Result> getItems() {
		return items;
	}
	
    @Override
	public String toString() {
		return "SEQUENCE";
	}

    @Override
	public String getLongDescription() {
		boolean wasFirst = false;
		StringBuffer sb = new StringBuffer();
		sb.append("SEQ(");
		for (Result r : items) {
			if (wasFirst) sb.append(";"); else wasFirst = true;
			sb.append(r.toString());
		}
		sb.append(")");
		return sb.toString();
	}
	
    @Override
	public int getType() {
		return ResultType.RES_SEQUENCE;
	}	
}
