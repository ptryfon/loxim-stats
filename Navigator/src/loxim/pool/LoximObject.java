package loxim.pool;

/**
 * Klasa abstrakcyjna reprezentująca obiekt w bazie LoXiM
 * @author tzr
 *
 */
public abstract class LoximObject {
	protected String ref;
	protected String name = null;
	protected LoximQuery query;
	
	protected void setRef(String ref) {
		this.ref = ref;
	}

	void setName(String name) {
		this.name = name;
	}
	
	public String getName() {
		return name;
	}
	
	/**
	 * Zwraca referencje do obiektu, jesli jest to obiekt tymczasowy, zwraca null
	 * @return
	 */
	public String getRef() {
		return ref;
	}
	
	/**
	 * Zwraca true, jesli dany obiekt jest obiektem tymczasowym
	 * @return
	 */
	public boolean isTemporary() {
		return ref == null;
	}
	
	@Override
	public boolean equals(Object object) {
		if (this.ref != null && object instanceof LoximObject) {
			LoximObject loximObject = (LoximObject)object;
			return this.ref.equals(loximObject.ref);
		} else {
			return super.equals(object);
		}
	}

	@Override
	public int hashCode() {
		if (this.ref != null) {
			return ref.hashCode();
		} else {
			return super.hashCode();
		}
	}
	
	public abstract String toString2();
	
	public String toString() {
		if (name == null) {
			return toString2();
		} else {
			return name + "(" + toString2() + ")";
		}
	}

	public LoximQuery getQuery() {
		return query;
	}

	public void setQuery(LoximQuery query) {
		this.query = query;
	}
}
