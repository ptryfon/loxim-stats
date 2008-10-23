package pl.edu.mimuw.loxim.protogen.lang.java.template.layers;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

import pl.edu.mimuw.loxim.protogen.lang.java.template.base_packages.ASCPingPackage;
import pl.edu.mimuw.loxim.protogen.lang.java.template.base_packages.ASCPongPackage;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackageInputStream;
import pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackageOutputStream;
import pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackagesFactory;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;

public class ProtocolLayer0 {

	private PackageInputStream pis;
	private PackageOutputStream pos;
	private Socket socket;

	public ProtocolLayer0(Socket s, PackagesFactory pf) throws IOException {
		this(s.getInputStream(), s.getOutputStream(), pf);
		socket=s;
	}

	public ProtocolLayer0(InputStream is, OutputStream os, PackagesFactory pf) throws IOException {
		pis = new PackageInputStream(is, pf);
		pos = new PackageOutputStream(os);
	}

	public void writePackage(Package p) throws ProtocolException {
		pos.writePackage(p);
	}

	public Package readPackage() throws ProtocolException {
		synchronized (pis) {
			Package p;
			do {
				p = pis.readPackage();
			} while (interpretePackage(p));
			return p;
		}
	}
	
	public void closeUnderlyingStreamsSocket() throws IOException
	{
		pis.closeUnderlyingStream();
		pos.closeUnderlyingStream();
		if(socket!=null)
			socket.close();
	}

	private boolean interpretePackage(Package p) throws ProtocolException {
		if(ASCPingPackage.class.isInstance(p))
		{
			pos.writePackage(new ASCPongPackage());
			return true;
		}
		return false;
	}

}
