package pl.tzr.transparent;

import java.util.Set;

import pl.tzr.driver.loxim.exception.SBQLException;

public interface TransparentSession {
		
	Set<Object> find(String query, Class desiredClass) throws SBQLException;
	
	void persist(Object object);
	
	void delete(Object object);
}
