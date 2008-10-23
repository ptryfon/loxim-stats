package pl.edu.mimuw.loxim.protogen.lang.java.template.ptools;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;

import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;
import pl.edu.mimuw.loxim.protogen.lang.java.template.util.ByteArrayUtil;

public abstract class Package {

	public Package() {
	}

	/**
	 * Metoda zwraca identyfikator paczki. Identyfikat paczki musi byæ unikatowy w obrêbie grupy paczek.
	 */
	public abstract long getPackageType();

	/**
	 * Metoda zapisuje paczkê do postaci tablicy bajtów. Paczka jest zapisywana w³±cznie z nag³ówkiem identyfikuj±cym paczkê i podaj±cym jej rozmiar.
	 * 
	 * @return
	 * @throws ProtocolException
	 */
	public byte[] serialize() throws ProtocolException {
		try {
			ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
			PackageOutputStreamWriter writer = new PackageOutputStreamWriter(byteArrayOutputStream);

			/* rezerwujemy miejsce na uzupe³nienie danych o paczce */
			writer.writeUint8((short) getPackageType());
			writer.writeUint32(0);

			serializeW(writer);
			byteArrayOutputStream.flush();
			byte[] res = byteArrayOutputStream.toByteArray();

			ByteArrayUtil.setOffsetUint8(res, 0, (short) getPackageType());
			ByteArrayUtil.setOffsetUint32(res, 1, res.length - 5);
			return res;
		} catch (IOException e) {
			throw new ProtocolException(e);
		}
	}

	/**
	 * Metoda wype³nia pola paczki w oparciu o zserializowane dane zawarte w tablicy danej parametrem b. Tablica "b" powinna zawieraæ pe³ny opis paczki -
	 * w³±cznie z jej nag³ówkiem.
	 */
	public void deserialize(byte[] b) throws ProtocolException {
		try {
			ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(b);
			PackageInputStreamReader reader = new PackageInputStreamReader(byteArrayInputStream);

			short package_type = reader.readUint8();
			if (package_type != getPackageType())
				throw new ProtocolException("Expected different package data. Got packageType: " + package_type + " expected: " + getPackageType());

			/* long size= */reader.readUint32();

			deserializeW(reader);

			byteArrayInputStream.close();
		} catch (IOException e) {
			throw new ProtocolException(e);
		}
	}

	/***********************************************************************************************************************************************************
	 * Metoda sprawdza, czy dana paczka jest równa paczce na której wywo³ujemy tê metodê. Za równe uznajemy te paczki, które s± tego samego typu i których pola
	 * s± równe co do zawarto¶ci.
	 * 
	 * Metoda powinna byæ nadpisana przez deklaracjê ka¿dej paczki.
	 */
	@Override
	public boolean equals(Object obj) {
		if (obj == null)
			return false;
		if (!Package.class.isInstance(obj))
			return false;
		Package p = (Package) obj;
		if (p.getPackageType() != getPackageType())
			return false;
		return true;
	}

	/**
	 * Metoda zapisuje do danego obiektu pisz±cego do strumienia wszystkie pola paczki (bez nag³ówka).
	 * 
	 * @param reader
	 * @throws ProtocolException
	 */
	public void deserializeContent(PackageInputStreamReader reader) throws ProtocolException {
		deserializeW(reader);
	}

	/**
	 * Metoda wczytuje paczkê z danego obiektu czytaj±cego strumieñ. Wczytywane s± wszystkie pola paczki (bez nag³ówka).
	 * 
	 * @param reader
	 * @throws ProtocolException
	 */
	public void serializeContent(PackageOutputStreamWriter writer) throws ProtocolException {
		serializeW(writer);
	}

	/**
	 * Metoda zapisuje do danego obiektu pisz±cego do strumienia wszystkie pola paczki (bez nag³ówka).
	 * 
	 * Metoda powinna byæ nadpisana przez deklaracjê ka¿dej paczki.
	 * 
	 * @param reader
	 * @throws ProtocolException
	 */

	protected void deserializeW(PackageInputStreamReader reader) throws ProtocolException {
	};

	/**
	 * Metoda wczytuje paczkê z danego obiektu czytaj±cego strumieñ. Wczytywane s± wszystkie pola paczki (bez nag³ówka).
	 * 
	 * Metoda powinna byæ nadpisana przez deklaracjê ka¿dej paczki.
	 * 
	 * @param reader
	 * @throws ProtocolException
	 */
	protected void serializeW(PackageOutputStreamWriter writer) throws ProtocolException {
	};
}
