package pl.edu.mimuw.loxim.protocol.tests;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;

import pl.edu.mimuw.loxim.protocol.packages.PackagesFactory;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackageInputStream;
import pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackageOutputStream;

import org.junit.Ignore;

@Ignore
public class TestRunnerRec {
	public static void main(String[] args) throws ProtocolException, IOException {
			ServerSocket ss=new ServerSocket(9876);
			Socket s=ss.accept();
			PackageInputStream pis=new PackageInputStream(s.getInputStream(),new PackagesFactory());
			TestPackages tp=new TestPackages();
			tp.readAndCheckPackagesI(pis);
	}
}
