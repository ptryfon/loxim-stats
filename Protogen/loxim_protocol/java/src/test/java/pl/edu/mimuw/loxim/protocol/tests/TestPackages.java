package pl.edu.mimuw.loxim.protocol.tests;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.EOFException;
import java.io.IOException;

import org.junit.Assert;
import org.junit.Test;

import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackageInputStream;
import pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackageOutputStream;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;

public class TestPackages {
	
	public byte[] writePackages() throws ProtocolException, IOException
	{
		ByteArrayOutputStream bos=new ByteArrayOutputStream();
		PackageOutputStream posw=new PackageOutputStream(bos);
		writePackagesI(posw);
		bos.close();
		return bos.toByteArray();
		
	}
	
	public void writePackagesI(PackageOutputStream posw) throws ProtocolException {
		Package p;
		short i=0;
		TestPackagesFactory tpf=new TestPackagesFactory();
		while((p=tpf.createPackage(i))!=null)
		{
			System.out.println("Writing package: "+i);
			System.out.flush();
			posw.writePackage(p);
			i++;
		}		
	}

	public void readAndCheckPackages(byte[] b) throws ProtocolException, IOException
	{
		//Reading
		ByteArrayInputStream bais=new ByteArrayInputStream(b);
		PackageInputStream pis=new PackageInputStream(bais,new pl.edu.mimuw.loxim.protocol.packages.PackagesFactory());
		readAndCheckPackagesI(pis);
		bais.close();
	}
	
	public void readAndCheckPackagesI(PackageInputStream pis) throws ProtocolException{
		short i=0;
		Package p;
		TestPackagesFactory tpf=new TestPackagesFactory();
		while((p=tpf.createPackage(i))!=null)
		{
			Package r=pis.readPackage();
			System.out.print("Reading package: "+i);
			if(p.equals(r))
			{
				System.out.println("[equels]");
			}else
			{
				System.out.println("[not equels] !!!");
				Assert.fail("Package nr: "+i+" not equals after reading and writing");
			};
			System.out.flush();
			i++;
		}
		try{
			pis.readPackage();
			Assert.fail("Expected exception (EOF)");
		}catch (ProtocolException e) {
			//	Expected exception
			Assert.assertTrue(EOFException.class.isInstance(e.getCause()));
		}		
	}

	@Test
	public void testReadWrite() throws Exception {
		
		byte[] b= writePackages();
		System.out.println("Written :"+b.length+" bytes");
		readAndCheckPackages(b);			
	}
}
