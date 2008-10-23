package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class Q_s_execution_finishedPackage extends Package {
		public final static short ID=(short)68;
	
    	/**
**/
    	private Long modAtomPointerCnt=new Long(0);
    	/**
**/
    	private Long delCnt=new Long(0);
    	/**
**/
    	private Long newRootsCnt=new Long(0);
    	/**
**/
    	private Long insertsCnt=new Long(0);
    	
	public Q_s_execution_finishedPackage()
	{
	}

		public Q_s_execution_finishedPackage(
		    	Long a_modAtomPointerCnt,
	        	Long a_delCnt,
	        	Long a_newRootsCnt,
	        	Long a_insertsCnt    	)
	{
        	modAtomPointerCnt=a_modAtomPointerCnt;
        	delCnt=a_delCnt;
        	newRootsCnt=a_newRootsCnt;
        	insertsCnt=a_insertsCnt;
    
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
	    			modAtomPointerCnt=reader.readVaruint();
					delCnt=reader.readVaruint();
					newRootsCnt=reader.readVaruint();
					insertsCnt=reader.readVaruint();
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
	    			writer.writeVaruint(modAtomPointerCnt);		
					writer.writeVaruint(delCnt);		
					writer.writeVaruint(newRootsCnt);		
					writer.writeVaruint(insertsCnt);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		Q_s_execution_finishedPackage p=(Q_s_execution_finishedPackage)obj;
	    if (!(((modAtomPointerCnt==p.getModAtomPointerCnt())||((modAtomPointerCnt!=null)&&(p.getModAtomPointerCnt()!=null)&&((modAtomPointerCnt).equals(p.getModAtomPointerCnt())))))) 
return false;
		if (!(((delCnt==p.getDelCnt())||((delCnt!=null)&&(p.getDelCnt()!=null)&&((delCnt).equals(p.getDelCnt())))))) 
return false;
		if (!(((newRootsCnt==p.getNewRootsCnt())||((newRootsCnt!=null)&&(p.getNewRootsCnt()!=null)&&((newRootsCnt).equals(p.getNewRootsCnt())))))) 
return false;
		if (!(((insertsCnt==p.getInsertsCnt())||((insertsCnt!=null)&&(p.getInsertsCnt()!=null)&&((insertsCnt).equals(p.getInsertsCnt())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public Long getModAtomPointerCnt(){return modAtomPointerCnt;};
		
    	public void setModAtomPointerCnt(Long a_modAtomPointerCnt){ modAtomPointerCnt=a_modAtomPointerCnt;};
		
        	public Long getDelCnt(){return delCnt;};
		
    	public void setDelCnt(Long a_delCnt){ delCnt=a_delCnt;};
		
        	public Long getNewRootsCnt(){return newRootsCnt;};
		
    	public void setNewRootsCnt(Long a_newRootsCnt){ newRootsCnt=a_newRootsCnt;};
		
        	public Long getInsertsCnt(){return insertsCnt;};
		
    	public void setInsertsCnt(Long a_insertsCnt){ insertsCnt=a_insertsCnt;};
		
    
	 
}


