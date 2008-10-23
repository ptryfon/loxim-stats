#ifndef PACKAGEBUFFERREADER_H_
#define PACKAGEBUFFERREADER_H_

#include <stdint.h>
#include "CharArray.h"

namespace protocol{

class PackageBufferReader{
	private:
		uint32_t pos;
		uint32_t size;
		const char *buffer;
		bool hasError;
	
	public: 
		/*Ten konstruktor nie tworzy kopii bufora*/
		PackageBufferReader(const char* a_buffer,uint32_t size);
		
		/*A ten te¿ nie tworzy kopii, ale zak³ada, ¿e w parametrze dostaje tre¶æ pakietu razem z 
		 *nag³ówkiem (pe³nym - 5-cio bajtowym)*/
		PackageBufferReader(const char* a_package);
		
		/*Ten destruktor nie zwalnia bufora*/
		virtual ~PackageBufferReader();

		int8_t readInt8();
		int16_t readInt16();
		int32_t readInt32();
		int64_t readInt64();
		
		uint8_t readUint8();
		uint16_t readUint16();
		uint32_t readUint32();
		uint64_t readUint64();
		
		int64_t readVaruint(bool &null);
		
		double readDouble();
		bool readBool();
		
		
		/**
		 * Alokuje (kopiuje) zwracan± warto¶æ - pamiêtaj by j± zwolniæ
		 * Zwraca bufor o znak 0 d³u¿szy ni¿ ten przes³any przez sieæ. 
		 * a_size - Nie uwzglêdnia tego znaku (czyli zwraca prawdziw± d³ugo¶æ przes³an± przez sieæ)
		 * */
		char* readSstring(uint32_t &a_size);
		
		/**
		 * Alokuje (kopiuje) zwracan± warto¶æ - pamiêtaj by j± zwolniæ
		 * Zwraca bufor o znak 0 d³u¿szy ni¿ ten przes³any przez sieæ. 
		 * a_size - Nie uwzglêdnia tego znaku (czyli zwraca prawdziw± d³ugo¶æ - przes³an± przez sieæ)
		 * */
		char* readString(uint32_t &a_size);
		
		/**
		 * Alokuje (kopiuje) zwracan± warto¶æ - pamiêtaj by j± zwolniæ
		 * */
		CharArray* readString();
		
		/**
		 * Alokuje (kopiuje) zwracan± warto¶æ - pamiêtaj by j± zwolniæ
		 * */
		CharArray* readSstring();
		
		/*is End Of Package*/
		bool isEOP(){return pos>=size;};
		
//================ Obs³uga problemów =============================
		/**
		 * Zaznacza, ¿e paczka jest uszkodzona.
		 * Przenosi kursor na koniec paczki.
		 */
		void markError(){hasError=true; pos=size;};
		bool hasERROR(){return hasError;};
		
		bool available_bytes(uint32_t need)
		{
			if(pos+need<=size) 
				return true;
				else{
					markError();
					return false;
				};
		};
		
};
}
#endif /*PACKAGEBUFFERREADER_H_*/
