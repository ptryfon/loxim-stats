package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class V_sc_sendvaluePackage extends Package {
		public final static short ID=(short)33;
	
    	/**
**/
    	private Long value_id=new Long(0);
    	/**
**/
    	private EnumSet<Send_value_flagsEnum> flags=EnumSet.noneOf(Send_value_flagsEnum.class);
    	/**
**/
    	private Long value_type=new Long(0);
    	/**
**/
    	private Package data=null;
    	
	public V_sc_sendvaluePackage()
	{
	}

		public V_sc_sendvaluePackage(
		    	Long a_value_id,
	        	EnumSet<Send_value_flagsEnum> a_flags,
	        	Long a_value_type,
	        	Package a_data    	)
	{
        	value_id=a_value_id;
        	flags=a_flags;
        	value_type=a_value_type;
        	data=a_data;
    
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
	    			value_id=reader.readVaruint();
					short tmp_flags=reader.readUint8();
flags=Send_value_flagsEnum.createEnumMapByValue(tmp_flags);
					value_type=reader.readVaruint();
					data=pl.edu.mimuw.loxim.protocol.packages_data.DataPackagesFactory.getInstance().createPackage(value_type);
data.deserializeContent(reader);
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
	    			writer.writeVaruint(value_id);		
					writer.writeUint8(Send_value_flagsEnum.toEnumMapValue(flags));		
					writer.writeVaruint(value_type);		
					data.serializeContent(writer);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		V_sc_sendvaluePackage p=(V_sc_sendvaluePackage)obj;
	    if (!(((value_id==p.getValue_id())||((value_id!=null)&&(p.getValue_id()!=null)&&((value_id).equals(p.getValue_id())))))) 
return false;
		if (!(((flags==p.getFlags())||((flags!=null)&&(p.getFlags()!=null)&&((flags).equals(p.getFlags())))))) 
return false;
		if (!(((value_type==p.getValue_type())||((value_type!=null)&&(p.getValue_type()!=null)&&((value_type).equals(p.getValue_type())))))) 
return false;
		if (!(((data==p.getData())||((data!=null)&&(p.getData()!=null)&&((data).equals(p.getData())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public Long getValue_id(){return value_id;};
		
    	public void setValue_id(Long a_value_id){ value_id=a_value_id;};
		
        	public EnumSet<Send_value_flagsEnum> getFlags(){return flags;};
		
    	public void setFlags(EnumSet<Send_value_flagsEnum> a_flags){ flags=a_flags;};
		
        	public Long getValue_type(){return value_type;};
		
    	public void setValue_type(Long a_value_type){ value_type=a_value_type;};
		
        	public Package getData(){return data;};
		
    	public void setData(Package a_data){ data=a_data;};
		
    
	 
}


