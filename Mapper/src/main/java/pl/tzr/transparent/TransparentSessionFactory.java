package pl.tzr.transparent;

/**
 * Session handler that allows us acces to the current session, and
 * if the session doesn't exists, creates new one.
 * @author Tomasz Rosiek
 *
 */
public interface TransparentSessionFactory {

	/**
	 * Creates new transparent session 
	 * @return
	 */
	TransparentSession getSession();
		
}
