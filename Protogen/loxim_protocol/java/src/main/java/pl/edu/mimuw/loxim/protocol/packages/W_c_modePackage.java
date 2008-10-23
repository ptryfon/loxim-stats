package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class W_c_modePackage extends Package {
		public final static short ID=(short)12;
	
    	/**
**/
    	private ModesEnum new_mode=null;
    	
	public W_c_modePackage()
	{
	}

		public W_c_modePackage(
		    	ModesEnum a_new_mode    	)
	{
        	new_mode=a_new_mode;
    
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
	    			java.math.BigInteger tmp_new_mode=reader.readUint64();
new_mode=ModesEnum.createEnumByValue(tmp_new_mode);
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
	    			writer.writeUint64(new_mode.getValue());		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		W_c_modePackage p=(W_c_modePackage)obj;
	    if (!(((new_mode==p.getNew_mode())||((new_mode!=null)&&(p.getNew_mode()!=null)&&((new_mode).equals(p.getNew_mode())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public ModesEnum getNew_mode(){return new_mode;};
		
    	public void setNew_mode(ModesEnum a_new_mode){ new_mode=a_new_mode;};
		
    
	 
}


