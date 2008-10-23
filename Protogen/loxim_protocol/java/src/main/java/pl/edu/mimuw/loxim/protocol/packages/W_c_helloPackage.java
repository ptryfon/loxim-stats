package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class W_c_helloPackage extends Package {
		public final static short ID=(short)10;
	
    	/**
**/
    	private long pid=0;
    	/**
**/
    	private String client_name=null;
    	/**
**/
    	private String client_version=null;
    	/**
**/
    	private String hostname=null;
    	/**
**/
    	private String language=null;
    	/**
**/
    	private CollationsEnum collation=null;
    	/**
**/
    	private byte timezone=0;
    	
	public W_c_helloPackage()
	{
	}

		public W_c_helloPackage(
		    	long a_pid,
	        	String a_client_name,
	        	String a_client_version,
	        	String a_hostname,
	        	String a_language,
	        	CollationsEnum a_collation,
	        	byte a_timezone    	)
	{
        	pid=a_pid;
        	client_name=a_client_name;
        	client_version=a_client_version;
        	hostname=a_hostname;
        	language=a_language;
        	collation=a_collation;
        	timezone=a_timezone;
    
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
	    			pid=reader.readInt64();
					client_name=reader.readSString();
					client_version=reader.readSString();
					hostname=reader.readSString();
					language=reader.readSString();
					java.math.BigInteger tmp_collation=reader.readUint64();
collation=CollationsEnum.createEnumByValue(tmp_collation);
					timezone=reader.readInt8();
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
	    			writer.writeInt64(pid);		
					writer.writeSString(client_name);		
					writer.writeSString(client_version);		
					writer.writeSString(hostname);		
					writer.writeSString(language);		
					writer.writeUint64(collation.getValue());		
					writer.writeInt8(timezone);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		W_c_helloPackage p=(W_c_helloPackage)obj;
	    if (!((pid==p.getPid()))) 
return false;
		if (!(((client_name==p.getClient_name())||((client_name!=null)&&(p.getClient_name()!=null)&&((client_name).equals(p.getClient_name())))))) 
return false;
		if (!(((client_version==p.getClient_version())||((client_version!=null)&&(p.getClient_version()!=null)&&((client_version).equals(p.getClient_version())))))) 
return false;
		if (!(((hostname==p.getHostname())||((hostname!=null)&&(p.getHostname()!=null)&&((hostname).equals(p.getHostname())))))) 
return false;
		if (!(((language==p.getLanguage())||((language!=null)&&(p.getLanguage()!=null)&&((language).equals(p.getLanguage())))))) 
return false;
		if (!(((collation==p.getCollation())||((collation!=null)&&(p.getCollation()!=null)&&((collation).equals(p.getCollation())))))) 
return false;
		if (!((timezone==p.getTimezone()))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public long getPid(){return pid;};
		
    	public void setPid(long a_pid){ pid=a_pid;};
		
        	public String getClient_name(){return client_name;};
		
    	public void setClient_name(String a_client_name){ client_name=a_client_name;};
		
        	public String getClient_version(){return client_version;};
		
    	public void setClient_version(String a_client_version){ client_version=a_client_version;};
		
        	public String getHostname(){return hostname;};
		
    	public void setHostname(String a_hostname){ hostname=a_hostname;};
		
        	public String getLanguage(){return language;};
		
    	public void setLanguage(String a_language){ language=a_language;};
		
        	public CollationsEnum getCollation(){return collation;};
		
    	public void setCollation(CollationsEnum a_collation){ collation=a_collation;};
		
        	public byte getTimezone(){return timezone;};
		
    	public void setTimezone(byte a_timezone){ timezone=a_timezone;};
		
    
	 
}


