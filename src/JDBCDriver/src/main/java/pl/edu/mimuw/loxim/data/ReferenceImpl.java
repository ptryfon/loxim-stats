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

	@Override
	public <T> T mapTo(Class<T> clazz) throws IllegalArgumentException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String toString() {
		return "ref(" + valueId + ")";
	}
}
