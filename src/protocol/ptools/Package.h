#ifndef PACKAGE_H_
#define PACKAGE_H_

#include <stdint.h>
#include <stdlib.h>
#include "PackageBufferWriter.h"
#include "PackageBufferReader.h"

namespace protocol{

class Package
{
	public:
		/*
		 * Metoda alokuje nowy bufor (do zwolnienia metod� free() i ustawia
		 * jego adres w zmiennej bufor. D�ugo�� bufora jest zwracana jako warto�� funkcji.
		 * 
		 * W zwr�conym buforze znajduje si� pakiet wraz z 5-cio bajtowym nag��wkiem.
		 * */
		virtual const int serialize(char** bufor);
		
		/**
		 * W buforze "data" nale�y umie�ci� tre�� pe�nego pakietu wraz z nag��wkiem 
		 * 
		 * Rozmiar bufora data powinien wynosi� conajmniej tylke bajt�w
		 * ile wynika z zapisanej w nag��wku pakietu d�ugo�ci. Kontrola tego 
		 * faktu nale�y do wywo�uj�cego t� metod� - i jej naruszenie mo�e prowadzi�
		 * do krytycznych problem�w bezpiecze�stwa i stabilno�ci. 
		 * 
		 * Zwraca informacj�, czy proces deserializacji si� powi�d�.
		 */
		virtual bool deserialize(char* data);
		virtual ~Package(){};
		
		virtual uint8_t getPackageType()=0;
		
		virtual bool equals(Package* package)=0;
		
		protected:
				virtual void serializeW(PackageBufferWriter *writter)=0;
				
				/*Ma zwr�ci� informacj�, czy proces deserializacji si� powi�d�*/
				virtual bool deserializeR(PackageBufferReader *reader)=0;
};
}
#endif /*PACKAGE_H_*/
