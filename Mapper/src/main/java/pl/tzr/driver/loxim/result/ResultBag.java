package pl.tzr.driver.loxim.result;

import java.util.LinkedList;
import java.util.List;


public class ResultBag extends ResultCollection {
	List<Result> items;
	public ResultBag() {
		items = new LinkedList<Result>();
	}
	@Override
    public List<Result> getItems() {
		return items;
	}
	
	@Override
    public String toString() {
		return "BAG";
	}	
	
	@Override
    public String getLongDescription() {
		boolean wasFirst = false;
		StringBuffer sb = new StringBuffer();
		sb.append("BAG(");		
		for (Result r : items) {
			if (wasFirst) sb.append(";"); else wasFirst = true;			
			sb.append(r.toString());			
		}
		sb.append(")");
		return sb.toString();
	}
	
	@Override
    public int getType() {
		return ResultType.RES_BAG;
	}
}
