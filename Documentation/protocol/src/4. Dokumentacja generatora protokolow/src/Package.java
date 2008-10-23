public abstract class Package {

	/**
	 * Metoda wype³nia pola paczki w oparciu o zserializowane dane zawarte w tablicy danej parametrem b. 
	 * Tablica "b" powinna zawieraæ pe³ny opis paczki - w³±cznie z jej nag³ówkiem.  
	 * */
	public void deserialize(byte[] b) throws ProtocolException;

	/**
	 * Metoda sprawdza, czy dana paczka jest równa paczce na której wywo³ujemy tê metodê. 
	 * Za równe uznajemy te paczki, które s± tego samego typu i których pola s± równe co do zawarto¶ci.
	 * 
	 * Metoda powinna byæ nadpisana przez deklaracjê ka¿dej paczki.
	 */
	public boolean equals(Object obj) {};

	/**
	 * Metoda zapisuje do danego obiektu pisz±cego do strumienia wszystkie pola paczki (bez nag³ówka).   
	 */
	public void deserializeContent(PackageInputStreamReader reader);

	/**
	 * Metoda wczytuje paczkê z danego obiektu czytaj±cego strumieñ. 
	 * Wczytywane s± wszystkie pola paczki (bez nag³ówka).   
	 */
	public void serializeContent(PackageOutputStreamWriter writer);

	/**
	 * Metoda zapisuje do danego obiektu pisz±cego do strumienia wszystkie pola paczki (bez nag³ówka).   
	 *
	 * Metoda powinna byæ nadpisana przez deklaracjê ka¿dej paczki. 
	 */
	protected abstract void deserializeW(PackageInputStreamReader reader)
			throws ProtocolException;

	/**
	 * Metoda wczytuje paczkê z danego obiektu czytaj±cego strumieñ. 
	 * Wczytywane s± wszystkie pola paczki (bez nag³ówka).
	 * 
	 * Metoda powinna byæ nadpisana przez deklaracjê ka¿dej paczki.
	 */
	protected abstract void serializeW(PackageOutputStreamWriter writer)
			throws ProtocolException;
}