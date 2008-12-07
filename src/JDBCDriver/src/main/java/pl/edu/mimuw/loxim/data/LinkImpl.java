package pl.edu.mimuw.loxim.data;

public class LinkImpl implements Link {

	private long valueId;
	
	@Override
	public long getValueId() {
		return valueId;
	}

	@Override
	public void setValueId(long valueId) {
		this.valueId = valueId;
	}

	@Override
	public <T> T mapTo(Class<T> clazz) throws IllegalArgumentException {
		// TODO Auto-generated method stub
		return null;
	}

}
