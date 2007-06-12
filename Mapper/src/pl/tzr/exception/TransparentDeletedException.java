package pl.tzr.exception;


public class TransparentDeletedException extends MappingException {

	public TransparentDeletedException(DeletedException e) {
		super(e);
	}
	
}
