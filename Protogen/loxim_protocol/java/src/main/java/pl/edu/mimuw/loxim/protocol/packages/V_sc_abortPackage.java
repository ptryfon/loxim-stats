package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class V_sc_abortPackage extends Package {
		public final static short ID=(short)35;
	
    	/**
**/
    	private Abort_reason_codesEnum reasonCode=null;
    	/**
**/
    	private String reasonString=null;
    	
	public V_sc_abortPackage()
	{
	}

		public V_sc_abortPackage(
		    	Abort_reason_codesEnum a_reasonCode,
	        	String a_reasonString    	)
	{
        	reasonCode=a_reasonCode;
        	reasonString=a_reasonString;
    
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
	    			long tmp_reasonCode=reader.readUint32();
reasonCode=Abort_reason_codesEnum.createEnumByValue(tmp_reasonCode);
					reasonString=reader.readString();
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
	    			writer.writeUint32(reasonCode.getValue());		
					writer.writeString(reasonString);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		V_sc_abortPackage p=(V_sc_abortPackage)obj;
	    if (!(((reasonCode==p.getReasonCode())||((reasonCode!=null)&&(p.getReasonCode()!=null)&&((reasonCode).equals(p.getReasonCode())))))) 
return false;
		if (!(((reasonString==p.getReasonString())||((reasonString!=null)&&(p.getReasonString()!=null)&&((reasonString).equals(p.getReasonString())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public Abort_reason_codesEnum getReasonCode(){return reasonCode;};
		
    	public void setReasonCode(Abort_reason_codesEnum a_reasonCode){ reasonCode=a_reasonCode;};
		
        	public String getReasonString(){return reasonString;};
		
    	public void setReasonString(String a_reasonString){ reasonString=a_reasonString;};
		
    
	 
}


