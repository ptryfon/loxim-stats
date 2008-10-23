package pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;

import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;


public class PackageInputStream {
	private InputStream inputStream;
	private PackageInputStreamReader pisr;
	private PackagesFactory packagesFactory;
	
	public PackageInputStream(InputStream is,PackagesFactory pf) {
		inputStream=is;
		pisr=new PackageInputStreamReader(inputStream);
		packagesFactory=pf;
	}
	
	public synchronized Package readPackage() throws ProtocolException
	{
		try{
			short package_id=pisr.readUint8();
			long package_size=pisr.readUint32();
			byte res[]=pisr.readNbytes((int)package_size);
			Package p=packagesFactory.createPackage(package_id);
			if(p==null)
				throw new ProtocolException("Cannot read from stream package with id:"+package_id);
			p.deserializeContent(new PackageInputStreamReader(new ByteArrayInputStream(res)));
			return p;
		}catch (IOException e) {
			throw new ProtocolException(e);
		}
	}

	public synchronized void closeUnderlyingStream() throws IOException {
		inputStream.close();		
	}

	
}
