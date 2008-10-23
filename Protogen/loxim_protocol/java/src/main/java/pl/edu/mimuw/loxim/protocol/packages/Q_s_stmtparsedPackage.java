package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class Q_s_stmtparsedPackage extends Package {
		public final static short ID=(short)65;
	
    	/**
**/
    	private java.math.BigInteger statement_id=java.math.BigInteger.ZERO;
    	/**
**/
    	private long paramsCnt=0;
    	
	public Q_s_stmtparsedPackage()
	{
	}

		public Q_s_stmtparsedPackage(
		    	java.math.BigInteger a_statement_id,
	        	long a_paramsCnt    	)
	{
        	statement_id=a_statement_id;
        	paramsCnt=a_paramsCnt;
    
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
	    			statement_id=reader.readUint64();
					paramsCnt=reader.readUint32();
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
	    			writer.writeUint64(statement_id);		
					writer.writeUint32(paramsCnt);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		Q_s_stmtparsedPackage p=(Q_s_stmtparsedPackage)obj;
	    if (!(((statement_id==p.getStatement_id())||((statement_id!=null)&&(p.getStatement_id()!=null)&&((statement_id).equals(p.getStatement_id())))))) 
return false;
		if (!((paramsCnt==p.getParamsCnt()))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public java.math.BigInteger getStatement_id(){return statement_id;};
		
    	public void setStatement_id(java.math.BigInteger a_statement_id){ statement_id=a_statement_id;};
		
        	public long getParamsCnt(){return paramsCnt;};
		
    	public void setParamsCnt(long a_paramsCnt){ paramsCnt=a_paramsCnt;};
		
    
	 
}


