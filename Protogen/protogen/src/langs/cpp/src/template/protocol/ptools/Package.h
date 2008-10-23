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
		 * Metoda alokuje nowy bufor (do zwolnienia metod± free() i ustawia
		 * jego adres w zmiennej bufor. D³ugo¶æ bufora jest zwracana jako warto¶æ funkcji.
		 * 
		 * W zwróconym buforze znajduje siê pakiet wraz z 5-cio bajtowym nag³ówkiem.
		 * */
		virtual const int serialize(char** bufor);
		
		/**
		 * W buforze "data" nale¿y umie¶ciæ tre¶æ pe³nego pakietu wraz z nag³ówkiem 
		 * 
		 * Rozmiar bufora data powinien wynosiæ conajmniej tylke bajtów
		 * ile wynika z zapisanej w nag³ówku pakietu d³ugo¶ci. Kontrola tego 
		 * faktu nale¿y do wywo³uj±cego tê metodê - i jej naruszenie mo¿e prowadziæ
		 * do krytycznych problemów bezpieczeñstwa i stabilno¶ci. 
		 * 
		 * Zwraca informacjê, czy proces deserializacji siê powiód³.
		 */
		virtual bool deserialize(char* data);
		virtual ~Package(){};
		
		virtual uint8_t getPackageType()=0;
		
		virtual bool equals(Package* package){return true;};//=0 (abstract)
		
		void serializeContent(PackageBufferWriter *writter){serializeW(writter);};
		bool deserializeContent(PackageBufferReader *reader){return deserializeR(reader);};

		static bool equal(Package* p1,Package* p2)
		{
			if (p1==p2)
				return true;
			if ((p1==NULL) || (p2==NULL))
				return false;
			return p1->equals(p2);
		}
		
		protected:
				virtual void serializeW(PackageBufferWriter *writter){};//=0 (abstract)
				
				/*Ma zwróciæ informacjê, czy proces deserializacji siê powiód³*/
				virtual bool deserializeR(PackageBufferReader *reader){return true;};//=0 (abstract)
};
}
#endif /*PACKAGE_H_*/
