package pl.tzr.driver.loxim.result;

import java.util.LinkedList;
import java.util.List;


public class ResultStruct extends ResultCollection {
	final List<Result> items = new LinkedList<Result>();
	
	public ResultStruct() {	
        /* Default constructor */
	}
	
	public ResultStruct(Result... newItems) {		
		for (Result newItem : newItems) {
			items.add(newItem);
		}		
	}

    @Override
	public List<Result> getItems() {
		return items;
	}
    
    @Override
	public String toString() {
		return "STRUCT";
	}
	
    @Override
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
	
	@Override
    public int getType() {
		return ResultType.RES_STRUCT;
	}	
}
