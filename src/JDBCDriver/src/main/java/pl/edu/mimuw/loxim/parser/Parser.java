package pl.edu.mimuw.loxim.parser;

/**
 * Interface for parsing from different query languages to SBQL.
 * 
 * @author Adam Michalik
 *
 * 2008-10-11
 */
public interface Parser {

	/**
	 * Parses <code>query</code> to SBQL.
	 * 
	 * @param query - query to be parsed
	 * @return parsed SBQL query
	 */
	public String parse(String query);
	
}
