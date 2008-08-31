package pl.edu.mimuw.loxim.jdbc;

import java.net.MalformedURLException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

class DatabaseURL {

	public static final String PROTOCOL_JDBC = "jdbc";
	public static final String PROTOCOL_SEPARATOR = ":";
	public static final String PROTOCOL_LOXIM = "loxim";
	public static final String PROTOCOL_PREFIX = PROTOCOL_JDBC + PROTOCOL_SEPARATOR + PROTOCOL_LOXIM + PROTOCOL_SEPARATOR;
	public static final String CONNECTION_SEPARATOR = "/";
	public static final String HOST_REGEX = "[^:" + CONNECTION_SEPARATOR + "]*?";
	public static final String PORT_REGEX = "\\p{Digit}{1,5}";
	public static final String DB_NAME_REGEX = ".*";
	
	/**
	 * <code>jdbc:loxim:&lt;host&gt;[:&lt;port&gt;]/&lt;database&gt;</code>
	 */
	public static final String PROTOCOL_REGEX = PROTOCOL_PREFIX + "("+ HOST_REGEX + ")(:("+ PORT_REGEX + "))?" + CONNECTION_SEPARATOR + "(" + DB_NAME_REGEX + ")";
	private static final Pattern protocolPattern = Pattern.compile(PROTOCOL_REGEX);

	public static ConnectionInfo parseURL(String jdbcURL) throws MalformedURLException {
		Matcher matcher = protocolPattern.matcher(jdbcURL);
		if (matcher.matches()) {
			String portString = matcher.group(3);
			int port = LoXiMProperties.defaultPort;
			if (portString != null && !portString.isEmpty()) {
				port = Integer.parseInt(portString);
			}
			return new ConnectionInfo(matcher.group(1), port, matcher.group(4));			
		}
		throw new MalformedURLException("The URL \"" + jdbcURL + "\" is malformed");
	}
	
}
