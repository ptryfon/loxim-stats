package pl.edu.mimuw.loxim.parser;

/**
 * Parser that does nothing
 * 
 * @author Adam Michalik
 *
 * 2008-10-11
 */
public class SBQLParser implements Parser {

	/**
	 * Does nothing
	 * @param query SBQL query
	 * @return the same query
	 */
	@Override
	public String parse(String query) {
		return query;
	}
	
}
