package pl.edu.mimuw.loxim.protocol.packages_data;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class TimePackage extends Package {
		public final static long ID=(long)11;
	
    	/**
**/
    	private short hour=0;
    	/**
**/
    	private short minuts=0;
    	/**
**/
    	private short sec=0;
    	/**
**/
    	private short milis=0;
    	
	public TimePackage()
	{
	}

		public TimePackage(
		    	short a_hour,
	        	short a_minuts,
	        	short a_sec,
	        	short a_milis    	)
	{
        	hour=a_hour;
        	minuts=a_minuts;
        	sec=a_sec;
        	milis=a_milis;
    
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
	    			hour=reader.readUint8();
					minuts=reader.readUint8();
					sec=reader.readUint8();
					milis=reader.readInt16();
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
	    			writer.writeUint8(hour);		
					writer.writeUint8(minuts);		
					writer.writeUint8(sec);		
					writer.writeInt16(milis);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		TimePackage p=(TimePackage)obj;
	    if (!((hour==p.getHour()))) 
return false;
		if (!((minuts==p.getMinuts()))) 
return false;
		if (!((sec==p.getSec()))) 
return false;
		if (!((milis==p.getMilis()))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public short getHour(){return hour;};
		
    	public void setHour(short a_hour){ hour=a_hour;};
		
        	public short getMinuts(){return minuts;};
		
    	public void setMinuts(short a_minuts){ minuts=a_minuts;};
		
        	public short getSec(){return sec;};
		
    	public void setSec(short a_sec){ sec=a_sec;};
		
        	public short getMilis(){return milis;};
		
    	public void setMilis(short a_milis){ milis=a_milis;};
		
    
	 
}


