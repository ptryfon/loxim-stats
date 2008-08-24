package pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams;

import java.io.Closeable;
import java.io.IOException;
import java.net.Socket;

import pl.edu.mimuw.loxim.protocol.packages.PackagesFactory;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;

public class PackageIO implements Closeable {

	private Socket socket;
	private PackageInputStream pis;
	private PackageOutputStream pos;
	
	public PackageIO(Socket socket) throws IOException {
		this.socket = socket;
		pos = new PackageOutputStream(socket.getOutputStream());
		pis = new PackageInputStream(socket.getInputStream(), PackagesFactory.getInstance());
	}

	public void write(Package pac) throws ProtocolException {
		pos.writePackage(pac);
	}
	
	public Package read() throws ProtocolException {
		return pis.readPackage();
	}
	
	@Override
	public void close() throws IOException {
		socket.close();
	}
}
