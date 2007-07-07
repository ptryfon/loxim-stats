package pl.tzr.test;

import pl.tzr.driver.loxim.LoximDatasourceImpl;
import pl.tzr.driver.loxim.SimpleConnection;
import pl.tzr.driver.loxim.result.Result;
import pl.tzr.driver.loxim.result.ResultBag;
import pl.tzr.driver.loxim.result.ResultReference;

public class Scratchpad {
	
	LoximDatasourceImpl datasource;
	
	public Scratchpad() {
		
		/* Create a datasource */
		datasource = new LoximDatasourceImpl();
		datasource.setHost("127.0.0.1");
		datasource.setPort(6543);
		datasource.setLogin("root");
		datasource.setPassword("");		
		
		
	}
	
	
	private void test1() throws Exception {

		SimpleConnection sc = datasource.getConnection();
		
		try {
			
			sc.beginTransaction();
			
			sc.execute("create ((5 as x, 6 as y) as point, (8 as x, 7 as y) as point) as line");
			
			Result result = sc.execute("line.point where x = 5");
			
			ResultReference ref = (ResultReference)(((ResultBag)result).getItems().get(0));
			
			Result result2 = sc.executeParam("line.point intersect ?", ref);
			
			System.out.println(((ResultBag)result2).getItems().get(0));
					
			
			sc.rollbackTransaction();
			
		} finally {
			datasource.release(sc);
		}
		
		
	}
	

	public static void main(String[] args) throws Exception {
		
		Scratchpad instance = new Scratchpad();
		
		instance.test1();
		
	}

}
