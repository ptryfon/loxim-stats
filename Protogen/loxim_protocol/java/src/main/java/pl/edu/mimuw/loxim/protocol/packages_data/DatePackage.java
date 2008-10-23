package pl.edu.mimuw.loxim.protocol.packages_data;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class DatePackage extends Package {
		public final static long ID=(long)10;
	
    	/**
**/
    	private short year=0;
    	/**
**/
    	private short month=0;
    	/**
**/
    	private short day=0;
    	
	public DatePackage()
	{
	}

		public DatePackage(
		    	short a_year,
	        	short a_month,
	        	short a_day    	)
	{
        	year=a_year;
        	month=a_month;
        	day=a_day;
    
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
	    			year=reader.readInt16();
					month=reader.readUint8();
					day=reader.readUint8();
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
	    			writer.writeInt16(year);		
					writer.writeUint8(month);		
					writer.writeUint8(day);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		DatePackage p=(DatePackage)obj;
	    if (!((year==p.getYear()))) 
return false;
		if (!((month==p.getMonth()))) 
return false;
		if (!((day==p.getDay()))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public short getYear(){return year;};
		
    	public void setYear(short a_year){ year=a_year;};
		
        	public short getMonth(){return month;};
		
    	public void setMonth(short a_month){ month=a_month;};
		
        	public short getDay(){return day;};
		
    	public void setDay(short a_day){ day=a_day;};
		
    
	 
}


