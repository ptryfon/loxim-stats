package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class W_c_passwordPackage extends Package {
		public final static short ID=(short)15;
	
    	/**
**/
    	private String login=null;
    	/**
**/
    	private String password=null;
    	
	public W_c_passwordPackage()
	{
	}

		public W_c_passwordPackage(
		    	String a_login,
	        	String a_password    	)
	{
        	login=a_login;
        	password=a_password;
    
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
	    			login=reader.readSString();
					password=reader.readSString();
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
	    			writer.writeSString(login);		
					writer.writeSString(password);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		W_c_passwordPackage p=(W_c_passwordPackage)obj;
	    if (!(((login==p.getLogin())||((login!=null)&&(p.getLogin()!=null)&&((login).equals(p.getLogin())))))) 
return false;
		if (!(((password==p.getPassword())||((password!=null)&&(p.getPassword()!=null)&&((password).equals(p.getPassword())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public String getLogin(){return login;};
		
    	public void setLogin(String a_login){ login=a_login;};
		
        	public String getPassword(){return password;};
		
    	public void setPassword(String a_password){ password=a_password;};
		
    
	 
}


