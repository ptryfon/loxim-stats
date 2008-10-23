package pl.edu.mimuw.loxim.protocol.packages_data;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class Uint64Package extends Package {
		public final static long ID=(long)7;
	
    	/**
**/
    	private java.math.BigInteger value=java.math.BigInteger.ZERO;
    	
	public Uint64Package()
	{
	}

		public Uint64Package(
		    	java.math.BigInteger a_value    	)
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
	    			value=reader.readUint64();
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
	    			writer.writeUint64(value);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		Uint64Package p=(Uint64Package)obj;
	    if (!(((value==p.getValue())||((value!=null)&&(p.getValue()!=null)&&((value).equals(p.getValue())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public java.math.BigInteger getValue(){return value;};
		
    	public void setValue(java.math.BigInteger a_value){ value=a_value;};
		
    
	 
}


