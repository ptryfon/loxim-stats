package pl.edu.mimuw.loxim.data;

import java.math.BigInteger;

public class ReferenceImpl implements Reference {

	private BigInteger valueId;
	
	@Override
	public BigInteger getValueId() {
		return valueId;
	}

	@Override
	public void setValueId(BigInteger valueId) {
		this.valueId = valueId;
	}

}
