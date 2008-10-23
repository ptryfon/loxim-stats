package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class Q_c_statementPackage extends Package {
		public final static short ID=(short)64;
	
    	/**
**/
    	private EnumSet<Statement_flagsEnum> flags=EnumSet.noneOf(Statement_flagsEnum.class);
    	/**
**/
    	private String statement=null;
    	
	public Q_c_statementPackage()
	{
	}

		public Q_c_statementPackage(
		    	EnumSet<Statement_flagsEnum> a_flags,
	        	String a_statement    	)
	{
        	flags=a_flags;
        	statement=a_statement;
    
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
	    			java.math.BigInteger tmp_flags=reader.readUint64();
flags=Statement_flagsEnum.createEnumMapByValue(tmp_flags);
					statement=reader.readString();
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
	    			writer.writeUint64(Statement_flagsEnum.toEnumMapValue(flags));		
					writer.writeString(statement);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		Q_c_statementPackage p=(Q_c_statementPackage)obj;
	    if (!(((flags==p.getFlags())||((flags!=null)&&(p.getFlags()!=null)&&((flags).equals(p.getFlags())))))) 
return false;
		if (!(((statement==p.getStatement())||((statement!=null)&&(p.getStatement()!=null)&&((statement).equals(p.getStatement())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public EnumSet<Statement_flagsEnum> getFlags(){return flags;};
		
    	public void setFlags(EnumSet<Statement_flagsEnum> a_flags){ flags=a_flags;};
		
        	public String getStatement(){return statement;};
		
    	public void setStatement(String a_statement){ statement=a_statement;};
		
    
	 
}


