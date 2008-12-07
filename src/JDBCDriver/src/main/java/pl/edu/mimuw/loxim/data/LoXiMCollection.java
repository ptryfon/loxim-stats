package pl.edu.mimuw.loxim.data;

import java.util.Collection;

public interface LoXiMCollection extends LoXiMObject {

	public Collection<Object> getData();
	public Class<?> getGlobalType();
	public void setGlobalType(Class<?> globalType);
}
