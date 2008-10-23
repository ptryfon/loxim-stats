package pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams;

import java.io.IOException;
import java.io.OutputStream;

import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;

public class PackageOutputStream {
	private OutputStream outputStream;
	
	public PackageOutputStream(OutputStream a_outputStream) {
		outputStream=a_outputStream;
	}
	
	public synchronized void writePackage(Package p) throws  ProtocolException
	{
		try{
			outputStream.write(p.serialize());
		}catch (IOException e) {
			throw new ProtocolException(e);
		}
	}

	public synchronized void closeUnderlyingStream() throws IOException {
		outputStream.close();		
	}
}
