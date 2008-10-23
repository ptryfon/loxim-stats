package pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams;

import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;

public interface PackagesFactory {

	Package createPackage(long package_id) throws ProtocolException ;

}
