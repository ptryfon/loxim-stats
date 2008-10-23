package pl.edu.mimuw.loxim.protocol.packages_data;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class Sint8Package extends Package {
		public final static long ID=(long)2;
	
    	/**
**/
    	private byte value=0;
    	
	public Sint8Package()
	{
	}

		public Sint8Package(
		    	byte a_value    	)
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
	    			value=reader.readInt8();
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
	    			writer.writeInt8(value);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		Sint8Package p=(Sint8Package)obj;
	    if (!((value==p.getValue()))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public byte getValue(){return value;};
		
    	public void setValue(byte a_value){ value=a_value;};
		
    
	 
}


