package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class A_sc_errorPackage extends Package {
		public final static short ID=(short)2;
	
    	/**
**/
    	private ErrorsEnum error_code=null;
    	/**
**/
    	private Long error_object_id=new Long(0);
    	/**
**/
    	private String description=null;
    	/**
**/
    	private long line=0;
    	/**
**/
    	private long col=0;
    	
	public A_sc_errorPackage()
	{
	}

		public A_sc_errorPackage(
		    	ErrorsEnum a_error_code,
	        	Long a_error_object_id,
	        	String a_description,
	        	long a_line,
	        	long a_col    	)
	{
        	error_code=a_error_code;
        	error_object_id=a_error_object_id;
        	description=a_description;
        	line=a_line;
        	col=a_col;
    
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
	    			long tmp_error_code=reader.readUint32();
error_code=ErrorsEnum.createEnumByValue(tmp_error_code);
					error_object_id=reader.readVaruint();
					description=reader.readSString();
					line=reader.readUint32();
					col=reader.readUint32();
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
	    			writer.writeUint32(error_code.getValue());		
					writer.writeVaruint(error_object_id);		
					writer.writeSString(description);		
					writer.writeUint32(line);		
					writer.writeUint32(col);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		A_sc_errorPackage p=(A_sc_errorPackage)obj;
	    if (!(((error_code==p.getError_code())||((error_code!=null)&&(p.getError_code()!=null)&&((error_code).equals(p.getError_code())))))) 
return false;
		if (!(((error_object_id==p.getError_object_id())||((error_object_id!=null)&&(p.getError_object_id()!=null)&&((error_object_id).equals(p.getError_object_id())))))) 
return false;
		if (!(((description==p.getDescription())||((description!=null)&&(p.getDescription()!=null)&&((description).equals(p.getDescription())))))) 
return false;
		if (!((line==p.getLine()))) 
return false;
		if (!((col==p.getCol()))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public ErrorsEnum getError_code(){return error_code;};
		
    	public void setError_code(ErrorsEnum a_error_code){ error_code=a_error_code;};
		
        	public Long getError_object_id(){return error_object_id;};
		
    	public void setError_object_id(Long a_error_object_id){ error_object_id=a_error_object_id;};
		
        	public String getDescription(){return description;};
		
    	public void setDescription(String a_description){ description=a_description;};
		
        	public long getLine(){return line;};
		
    	public void setLine(long a_line){ line=a_line;};
		
        	public long getCol(){return col;};
		
    	public void setCol(long a_col){ col=a_col;};
		
    
	 
}


