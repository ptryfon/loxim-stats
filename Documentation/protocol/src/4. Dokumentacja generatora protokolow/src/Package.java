public abstract class Package {

	/**
	 * Metoda wype�nia pola paczki w oparciu o zserializowane dane zawarte w tablicy danej parametrem b. 
	 * Tablica "b" powinna zawiera� pe�ny opis paczki - w��cznie z jej nag��wkiem.  
	 * */
	public void deserialize(byte[] b) throws ProtocolException;

	/**
	 * Metoda sprawdza, czy dana paczka jest r�wna paczce na kt�rej wywo�ujemy t� metod�. 
	 * Za r�wne uznajemy te paczki, kt�re s� tego samego typu i kt�rych pola s� r�wne co do zawarto�ci.
	 * 
	 * Metoda powinna by� nadpisana przez deklaracj� ka�dej paczki.
	 */
	public boolean equals(Object obj) {};

	/**
	 * Metoda zapisuje do danego obiektu pisz�cego do strumienia wszystkie pola paczki (bez nag��wka).   
	 */
	public void deserializeContent(PackageInputStreamReader reader);

	/**
	 * Metoda wczytuje paczk� z danego obiektu czytaj�cego strumie�. 
	 * Wczytywane s� wszystkie pola paczki (bez nag��wka).   
	 */
	public void serializeContent(PackageOutputStreamWriter writer);

	/**
	 * Metoda zapisuje do danego obiektu pisz�cego do strumienia wszystkie pola paczki (bez nag��wka).   
	 *
	 * Metoda powinna by� nadpisana przez deklaracj� ka�dej paczki. 
	 */
	protected abstract void deserializeW(PackageInputStreamReader reader)
			throws ProtocolException;

	/**
	 * Metoda wczytuje paczk� z danego obiektu czytaj�cego strumie�. 
	 * Wczytywane s� wszystkie pola paczki (bez nag��wka).
	 * 
	 * Metoda powinna by� nadpisana przez deklaracj� ka�dej paczki.
	 */
	protected abstract void serializeW(PackageOutputStreamWriter writer)
			throws ProtocolException;
}