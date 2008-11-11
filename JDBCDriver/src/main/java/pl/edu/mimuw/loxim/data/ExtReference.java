package pl.edu.mimuw.loxim.data;

import java.math.BigInteger;

public interface ExtReference extends LoXiMObject {

	public BigInteger getStamp();
	public void setStamp(BigInteger stamp);
	public BigInteger getValueId();
	public void setValueId(BigInteger valueId);
	
}
