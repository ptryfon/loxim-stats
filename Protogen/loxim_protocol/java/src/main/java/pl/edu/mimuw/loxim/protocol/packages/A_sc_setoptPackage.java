package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class A_sc_setoptPackage extends Package {
		public final static short ID=(short)4;
	
    	/**
**/
    	private String key=null;
    	/**
**/
    	private String value=null;
    	
	public A_sc_setoptPackage()
	{
	}

		public A_sc_setoptPackage(
		    	String a_key,
	        	String a_value    	)
	{
        	key=a_key;
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
	    			key=reader.readSString();
					value=reader.readString();
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
	    			writer.writeSString(key);		
					writer.writeString(value);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		A_sc_setoptPackage p=(A_sc_setoptPackage)obj;
	    if (!(((key==p.getKey())||((key!=null)&&(p.getKey()!=null)&&((key).equals(p.getKey())))))) 
return false;
		if (!(((value==p.getValue())||((value!=null)&&(p.getValue()!=null)&&((value).equals(p.getValue())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public String getKey(){return key;};
		
    	public void setKey(String a_key){ key=a_key;};
		
        	public String getValue(){return value;};
		
    	public void setValue(String a_value){ value=a_value;};
		
    
	 
}


