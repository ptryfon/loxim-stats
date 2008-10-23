package pl.edu.mimuw.loxim.protocol.packages_data;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class DatetimetzPackage extends Package {
		public final static long ID=(long)14;
	
    	/**
**/
    	private DatePackage date=null;
    	/**
**/
    	private TimePackage time=null;
    	/**
**/
    	private byte tz=0;
    	
	public DatetimetzPackage()
	{
	}

		public DatetimetzPackage(
		    	DatePackage a_date,
	        	TimePackage a_time,
	        	byte a_tz    	)
	{
        	date=a_date;
        	time=a_time;
        	tz=a_tz;
    
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
	    			date=new DatePackage();
date.deserializeContent(reader);
					time=new TimePackage();
time.deserializeContent(reader);
					tz=reader.readInt8();
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
	    			date.serializeContent(writer);
		
					time.serializeContent(writer);
		
					writer.writeInt8(tz);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		DatetimetzPackage p=(DatetimetzPackage)obj;
	    if (!(((date==p.getDate())||((date!=null)&&(p.getDate()!=null)&&((date).equals(p.getDate())))))) 
return false;
		if (!(((time==p.getTime())||((time!=null)&&(p.getTime()!=null)&&((time).equals(p.getTime())))))) 
return false;
		if (!((tz==p.getTz()))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public DatePackage getDate(){return date;};
		
    	public void setDate(DatePackage a_date){ date=a_date;};
		
        	public TimePackage getTime(){return time;};
		
    	public void setTime(TimePackage a_time){ time=a_time;};
		
        	public byte getTz(){return tz;};
		
    	public void setTz(byte a_tz){ tz=a_tz;};
		
    
	 
}


