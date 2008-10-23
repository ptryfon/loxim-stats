package pl.edu.mimuw.loxim.protocol.packages_data;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class External_refPackage extends Package {
		public final static long ID=(long)135;
	
    	/**
**/
    	private java.math.BigInteger valueId=java.math.BigInteger.ZERO;
    	/**
**/
    	private java.math.BigInteger stamp=java.math.BigInteger.ZERO;
    	
	public External_refPackage()
	{
	}

		public External_refPackage(
		    	java.math.BigInteger a_valueId,
	        	java.math.BigInteger a_stamp    	)
	{
        	valueId=a_valueId;
        	stamp=a_stamp;
    
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
					stamp=reader.readUint64();
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
					writer.writeUint64(stamp);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		External_refPackage p=(External_refPackage)obj;
	    if (!(((valueId==p.getValueId())||((valueId!=null)&&(p.getValueId()!=null)&&((valueId).equals(p.getValueId())))))) 
return false;
		if (!(((stamp==p.getStamp())||((stamp!=null)&&(p.getStamp()!=null)&&((stamp).equals(p.getStamp())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public java.math.BigInteger getValueId(){return valueId;};
		
    	public void setValueId(java.math.BigInteger a_valueId){ valueId=a_valueId;};
		
        	public java.math.BigInteger getStamp(){return stamp;};
		
    	public void setStamp(java.math.BigInteger a_stamp){ stamp=a_stamp;};
		
    
	 
}


