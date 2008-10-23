package pl.edu.mimuw.loxim.protocol.packages_data;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class DoublePackage extends Package {
		public final static long ID=(long)17;
	
    	/**
**/
    	private double value=0.0;
    	
	public DoublePackage()
	{
	}

		public DoublePackage(
		    	double a_value    	)
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
	    			value=reader.readDouble();
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
	    			writer.writeDouble(value);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		DoublePackage p=(DoublePackage)obj;
	    if (!((value==p.getValue()))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public double getValue(){return value;};
		
    	public void setValue(double a_value){ value=a_value;};
		
    
	 
}


