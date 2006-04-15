package szbd.client;

import szbd.SBQLException;
import szbd.client.result.Result;

public interface Connection {
	
	public Result execute(String query) throws SBQLException;
	public void close() throws SBQLException;
}