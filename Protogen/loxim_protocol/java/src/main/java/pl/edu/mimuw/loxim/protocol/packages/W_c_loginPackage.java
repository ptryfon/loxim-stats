package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class W_c_loginPackage extends Package {
		public final static short ID=(short)13;
	
    	/**
**/
    	private Auth_methodsEnum auth_method=null;
    	
	public W_c_loginPackage()
	{
	}

		public W_c_loginPackage(
		    	Auth_methodsEnum a_auth_method    	)
	{
        	auth_method=a_auth_method;
    
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
	    			java.math.BigInteger tmp_auth_method=reader.readUint64();
auth_method=Auth_methodsEnum.createEnumByValue(tmp_auth_method);
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
	    			writer.writeUint64(auth_method.getValue());		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		W_c_loginPackage p=(W_c_loginPackage)obj;
	    if (!(((auth_method==p.getAuth_method())||((auth_method!=null)&&(p.getAuth_method()!=null)&&((auth_method).equals(p.getAuth_method())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public Auth_methodsEnum getAuth_method(){return auth_method;};
		
    	public void setAuth_method(Auth_methodsEnum a_auth_method){ auth_method=a_auth_method;};
		
    
	 
}


