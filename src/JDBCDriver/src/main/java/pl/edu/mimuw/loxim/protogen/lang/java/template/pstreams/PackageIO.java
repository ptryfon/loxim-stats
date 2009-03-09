package pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams;

import java.io.Closeable;
import java.io.IOException;
import java.net.Socket;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import pl.edu.mimuw.loxim.jdbc.LoXiMConnectionImpl;
import pl.edu.mimuw.loxim.protocol.packages.PackagesFactory;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;

public class PackageIO implements Closeable {

	private static final Log log = LogFactory.getLog(PackageIO.class);
	
	private Socket socket;
	private PackageInputStream pis;
	private PackageOutputStream pos;
	
	public PackageIO(Socket socket) throws IOException {
		this.socket = socket;
		pos = new PackageOutputStream(socket.getOutputStream());
		pis = new PackageInputStream(socket.getInputStream(), PackagesFactory.getInstance());
	}

	public void write(Package pac) throws ProtocolException {
		log.debug("Writing package " + pac.getClass().getSimpleName());
		pos.writePackage(pac);
	}
	
	public Package read() throws ProtocolException {
		Package pac = pis.readPackage();
		log.debug("Read package " + pac.getClass().getSimpleName());
		return pac;
	}
	
	@Override
	public void close() throws IOException {
		socket.close();
	}
}
