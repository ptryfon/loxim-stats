package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class A_sc_byePackage extends Package {
		public final static short ID=(short)3;
	
    	/**
**/
    	private Bye_reasonsEnum reason_code=null;
    	/**
**/
    	private String reason_description=null;
    	
	public A_sc_byePackage()
	{
	}

		public A_sc_byePackage(
		    	Bye_reasonsEnum a_reason_code,
	        	String a_reason_description    	)
	{
        	reason_code=a_reason_code;
        	reason_description=a_reason_description;
    
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
	    			long tmp_reason_code=reader.readUint32();
reason_code=Bye_reasonsEnum.createEnumByValue(tmp_reason_code);
					reason_description=reader.readString();
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
	    			writer.writeUint32(reason_code.getValue());		
					writer.writeString(reason_description);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		A_sc_byePackage p=(A_sc_byePackage)obj;
	    if (!(((reason_code==p.getReason_code())||((reason_code!=null)&&(p.getReason_code()!=null)&&((reason_code).equals(p.getReason_code())))))) 
return false;
		if (!(((reason_description==p.getReason_description())||((reason_description!=null)&&(p.getReason_description()!=null)&&((reason_description).equals(p.getReason_description())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public Bye_reasonsEnum getReason_code(){return reason_code;};
		
    	public void setReason_code(Bye_reasonsEnum a_reason_code){ reason_code=a_reason_code;};
		
        	public String getReason_description(){return reason_description;};
		
    	public void setReason_description(String a_reason_description){ reason_description=a_reason_description;};
		
    
	 
}


