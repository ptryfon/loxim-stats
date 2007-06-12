package pl.tzr.driver.loxim;

import pl.tzr.driver.loxim.exception.SBQLException;

public interface LoximDatasource {
	
	SimpleConnection getConnection() throws SBQLException;

}
