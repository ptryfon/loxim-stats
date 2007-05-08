package pl.tzr.transparent.proxy.handler;

/**
 * Wyjątek rzucany, gdy typ danych w bazie danych jest niewłaściwy
 * w stosunku do spodziewanego typu danych
 * @author Tomasz Rosiek
 *
 */
public class InvalidDataStructure extends RuntimeException {

	public InvalidDataStructure() {
		// TODO Auto-generated constructor stub
	}

	public InvalidDataStructure(String arg0) {
		super(arg0);
		// TODO Auto-generated constructor stub
	}

	public InvalidDataStructure(Throwable arg0) {
		super(arg0);
		// TODO Auto-generated constructor stub
	}

	public InvalidDataStructure(String arg0, Throwable arg1) {
		super(arg0, arg1);
		// TODO Auto-generated constructor stub
	}

}
