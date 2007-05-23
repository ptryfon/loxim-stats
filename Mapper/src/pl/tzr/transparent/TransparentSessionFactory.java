package pl.tzr.transparent;

/**
 * Session handler that allows us acces to the current session, and
 * if the session doesn't exists, creates new one.
 * @author Tomasz Rosiek
 *
 */
public interface TransparentSessionFactory {

	/**
	 * Returns current transparent session, if transparent session doesn't
	 * exist creates new one 
	 * @return
	 */
	TransparentSession getCurrentSession();
	
	/**
	 * Closes current transparent session in any exists
	 */
	void closeCurrentSession();
	
}
