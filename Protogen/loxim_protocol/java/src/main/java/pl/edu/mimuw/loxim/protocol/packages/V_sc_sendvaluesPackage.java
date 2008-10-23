package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;


import pl.edu.mimuw.loxim.protocol.enums.*;


public  class V_sc_sendvaluesPackage extends Package {
		public final static short ID=(short)32;
	
    	/**
**/
    	private Long rootValueId=new Long(0);
    	/**
**/
    	private Long oBundlesCount=new Long(0);
    	/**
**/
    	private Long oBidCount=new Long(0);
    	/**
**/
    	private Long pVidCount=new Long(0);
    	
	public V_sc_sendvaluesPackage()
	{
	}

		public V_sc_sendvaluesPackage(
		    	Long a_rootValueId,
	        	Long a_oBundlesCount,
	        	Long a_oBidCount,
	        	Long a_pVidCount    	)
	{
        	rootValueId=a_rootValueId;
        	oBundlesCount=a_oBundlesCount;
        	oBidCount=a_oBidCount;
        	pVidCount=a_pVidCount;
    
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
	    			rootValueId=reader.readVaruint();
					oBundlesCount=reader.readVaruint();
					oBidCount=reader.readVaruint();
					pVidCount=reader.readVaruint();
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
	    			writer.writeVaruint(rootValueId);		
					writer.writeVaruint(oBundlesCount);		
					writer.writeVaruint(oBidCount);		
					writer.writeVaruint(pVidCount);		
				}catch(Exception e)
		{
			throw new ProtocolException(e);
		}	
			}
	
	@Override
	public boolean equals(Object obj) {
		if(!super.equals(obj))
			return false;
		V_sc_sendvaluesPackage p=(V_sc_sendvaluesPackage)obj;
	    if (!(((rootValueId==p.getRootValueId())||((rootValueId!=null)&&(p.getRootValueId()!=null)&&((rootValueId).equals(p.getRootValueId())))))) 
return false;
		if (!(((oBundlesCount==p.getOBundlesCount())||((oBundlesCount!=null)&&(p.getOBundlesCount()!=null)&&((oBundlesCount).equals(p.getOBundlesCount())))))) 
return false;
		if (!(((oBidCount==p.getOBidCount())||((oBidCount!=null)&&(p.getOBidCount()!=null)&&((oBidCount).equals(p.getOBidCount())))))) 
return false;
		if (!(((pVidCount==p.getPVidCount())||((pVidCount!=null)&&(p.getPVidCount()!=null)&&((pVidCount).equals(p.getPVidCount())))))) 
return false;
				return true;
	}
	
//================= GETTERS AND SETTERS ===================

        	public Long getRootValueId(){return rootValueId;};
		
    	public void setRootValueId(Long a_rootValueId){ rootValueId=a_rootValueId;};
		
        	public Long getOBundlesCount(){return oBundlesCount;};
		
    	public void setOBundlesCount(Long a_oBundlesCount){ oBundlesCount=a_oBundlesCount;};
		
        	public Long getOBidCount(){return oBidCount;};
		
    	public void setOBidCount(Long a_oBidCount){ oBidCount=a_oBidCount;};
		
        	public Long getPVidCount(){return pVidCount;};
		
    	public void setPVidCount(Long a_pVidCount){ pVidCount=a_pVidCount;};
		
    
	 
}


