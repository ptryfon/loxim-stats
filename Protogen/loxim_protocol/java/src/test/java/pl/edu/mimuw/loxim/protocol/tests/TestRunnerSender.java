package  pl.edu.mimuw.loxim.protocol.tests;

import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackageOutputStream;

import org.junit.Ignore;

@Ignore
public class TestRunnerSender {
	public static void main(String[] args) throws UnknownHostException, IOException, ProtocolException {
		Socket s=new Socket("localhost",9876);
		PackageOutputStream pos=new PackageOutputStream(s.getOutputStream());
		TestPackages tp=new TestPackages();
		tp.writePackagesI(pos);
				
	}
}
