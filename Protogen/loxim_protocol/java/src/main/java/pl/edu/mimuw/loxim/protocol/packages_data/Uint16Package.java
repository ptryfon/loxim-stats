package pl.edu.mimuw.loxim.protocol.packages_data;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class Uint16Package extends Package {
		public final static long ID=(long)3;
	
    	/**
**/
    	private int value=0;
    	
	public Uint16Package()
	{
	}

		public Uint16Package(
		    	int a_value    	)
	{
        	value=a_value;
    
	}
		
		@Override
	public long getPackageType() {
		return (long)ID;
	}
		
	@Override
	protected void deserializeW(PackageInputStreamReader reader)
			throws ProtocolException {
		super.deserializeW(reader);
		
				try{
	    			value=reader.readUint16();
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}
			}
	
	@Override
	protected void serializeW(PackageOutputStreamWriter writer)
			throws ProtocolException {
		super.serializeW(writer);
				try{
	    			writer.writeUint16(value);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		Uint16Package p=(Uint16Package)obj;
	    if (!((value==p.getValue()))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public int getValue(){return value;};
		
    	public void setValue(int a_value){ value=a_value;};
		
    
	 
}


