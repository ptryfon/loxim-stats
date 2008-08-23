package pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams;

import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;

public class PackageIO {

	private PackageInputStream pis;
	private PackageOutputStream pos;
	
	public PackageIO(PackageInputStream pis, PackageOutputStream pos) {
		this.pis = pis;
		this.pos = pos;
	}

	public void write(Package pac) throws ProtocolException {
		pos.writePackage(pac);
	}
	
	public Package read() throws ProtocolException {
		return pis.readPackage();
	}
}
