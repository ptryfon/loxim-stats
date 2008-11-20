package pl.edu.mimuw.loxim.data;

import java.math.BigInteger;

public class ExtReferenceImpl implements ExtReference {

	private BigInteger stamp;
	private BigInteger valueId;
	
	@Override
	public BigInteger getStamp() {
		return stamp;
	}

	@Override
	public BigInteger getValueId() {
		return valueId;
	}

	@Override
	public void setStamp(BigInteger stamp) {
		this.stamp = stamp;
	}

	@Override
	public void setValueId(BigInteger valueId) {
		this.valueId = valueId;
	}

}
