package szbd.client.result;

import java.util.LinkedList;
import java.util.List;


public class ResultSequence implements ResultCollection {
	List<Result> items;
	
	public ResultSequence() {
		items = new LinkedList<Result>();
	}

	public List<Result> getItems() {
		return items;
	}

	public String toString() {
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
}
