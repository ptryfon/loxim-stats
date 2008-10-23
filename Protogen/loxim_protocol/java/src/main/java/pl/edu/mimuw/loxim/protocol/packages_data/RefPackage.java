package pl.edu.mimuw.loxim.protocol.packages_data;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class RefPackage extends Package {
		public final static long ID=(long)134;
	
    	/**
**/
    	private java.math.BigInteger valueId=java.math.BigInteger.ZERO;
    	
	public RefPackage()
	{
	}

		public RefPackage(
		    	java.math.BigInteger a_valueId    	)
	{
        	valueId=a_valueId;
    
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
	    			valueId=reader.readUint64();
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
	    			writer.writeUint64(valueId);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		RefPackage p=(RefPackage)obj;
	    if (!(((valueId==p.getValueId())||((valueId!=null)&&(p.getValueId()!=null)&&((valueId).equals(p.getValueId())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public java.math.BigInteger getValueId(){return valueId;};
		
    	public void setValueId(java.math.BigInteger a_valueId){ valueId=a_valueId;};
		
    
	 
}


