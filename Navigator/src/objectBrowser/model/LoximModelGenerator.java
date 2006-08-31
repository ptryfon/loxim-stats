package objectBrowser.model;

import objectBrowser.driver.loxim.result.Result;
import objectBrowser.driver.loxim.result.ResultBag;
import objectBrowser.driver.loxim.result.ResultBinder;
import objectBrowser.driver.loxim.result.ResultReference;
import objectBrowser.driver.loxim.result.ResultStruct;

public class LoximModelGenerator {
	public DataObject buildObject(Result res, String name) {
		if (res instanceof ResultBinder) {
			return buildObject((ResultBinder)res, name);
		} else if (res instanceof ResultStruct) {
			return buildObject((ResultStruct)res, name);
		} else if (res instanceof ResultReference) {
			return new ReferenceDataObject(name, ((ResultReference)res).getRef());						
		} else 
			return new SimpleDataObject(name, res.getLongDescription());
	}
	
	public DataObject buildObject(ResultBinder res, String name) {
		if (res.getValue() instanceof ResultStruct) {
			DataObject obj = buildObject((ResultStruct)res.getValue(), res.getKey());
			return obj;
		} else if (res.getValue() instanceof ResultReference) {
			return new ReferenceDataObject(res.getKey(), ((ResultReference)res.getValue()).getRef());
		}
		SimpleDataObject obj = new SimpleDataObject(res.getKey(), res.getValue().toString());
		return obj;
	}
	
	public DataObject buildObject(ResultStruct res, String name) {
		CompositeDataObject obj = new CompositeDataObject(name);
		for (Result child : res.getItems()) {
			obj.addChild(buildObject(child, "<unnamed>"));
		}
		return obj;
	}	
	
	
	public Diagram makeModel(Result res) {
		Diagram pool = new Diagram();
		
		ResultNode rn = new ResultNode();		
		pool.getObjects().add(rn);
		
		if (res instanceof ResultBag) {
			for (Result child : ((ResultBag)res).getItems()) {
				DataObject obj = buildObject(child, "<result>");				
				pool.getObjects().add(obj);
				
				Relationship r1 = new Relationship();				
				rn.connectInput(r1);
				obj.connectOutput(r1);							
			}
		}
			
		
		return pool;
	}
}
