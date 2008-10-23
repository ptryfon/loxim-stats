package pl.edu.mimuw.loxim.protocol.packages_data;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class LinkPackage extends Package {
		public final static long ID=(long)129;
	
    	/**
**/
    	private Long valueId=new Long(0);
    	
	public LinkPackage()
	{
	}

		public LinkPackage(
		    	Long a_valueId    	)
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
	    			valueId=reader.readVaruint();
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
	    			writer.writeVaruint(valueId);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		LinkPackage p=(LinkPackage)obj;
	    if (!(((valueId==p.getValueId())||((valueId!=null)&&(p.getValueId()!=null)&&((valueId).equals(p.getValueId())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public Long getValueId(){return valueId;};
		
    	public void setValueId(Long a_valueId){ valueId=a_valueId;};
		
    
	 
}


