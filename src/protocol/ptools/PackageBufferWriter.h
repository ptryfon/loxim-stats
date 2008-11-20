#ifndef PACKAGEBUFFER_H_
#define PACKAGEBUFFER_H_

#include <stdint.h>
#include <protocol/ptools/StringBufferStack.h>
#include <protocol/ptools/CharArray.h>

namespace protocol{

/**
 * 
 * 
 */
class PackageBufferWriter
{
private:
	/**
	 *  Stosu buforów. W g³owie stosu znajduje siê ostatnio dodany "podbufor". 
	 */
	StringBufferStack *stringBufferHead;
	
	/**
	 * Aktualna ilo¶æ wszystkich wpisanych znaków.  
	 */
	int size; 


public:
	PackageBufferWriter();
	virtual ~PackageBufferWriter();
	
	/**
	 * Pobiera aktualny bufor utworzony przez klasê (jako wska¼nik
	 * do pamiêci zawartej w klasie). Automatycznie wywo³uje flush. 
	 */
	const char* getBuffer();
	
	const char* getPackageBuffer(uint8_t packageType);
	
	/**
	 * Zwraca aktualn± d³ugo¶æ (w bajtach) utworzonego bufora. 
	 * Zwrócona warto¶æ nie obejmuje 5 bajtowego nag³ówka utworzonego
	 * na pocz±tku bufora - wiêc musisz te 5 bajtów doliczyæ rêcznie.
	 */
	 int getSize(); 
	
	/**
	 * Powyduje uporz±dkowanie wszystkich fragmentów
	 * i uformowanie ich w bufor wynikowy, a nastêpnie
	 * zwolnienie pomiêci z tych fragmentów pomocniczych.  
	 * */
	void flush();
	
	/**
	 * Czy¶ci zupe³nie bufor i wszystkie zmienne pomocnicze.
	 * W praktyce oznacza rozpoczêcie przygotowywania nowej paczki.
	 */
	void clear();


//================= TYPY CA£KOWITOLICZBOWE =============================
	/**
	 * Zwraca liczbê bajtów zapisanych w buforze
	 */
	uint32_t writeInt8(int8_t value); 
	uint32_t writeInt16(int16_t value);
	uint32_t writeInt32(int32_t value);
	uint32_t writeInt64(int64_t value);
	uint32_t writeUint8(uint8_t value); 
	uint32_t writeUint16(uint16_t value);
	uint32_t writeUint32(uint32_t value);
	uint32_t writeUint64(uint64_t value);
	
	uint32_t writeVaruint(uint64_t value);
	uint32_t writeVaruintNull();
	
//======================== Napisy ========================================
   
   /*Je¶li size<0 to badamy d³ugo¶æ ³añcucha str. 
    * size powinien byæ d³ugo¶ci± napisu wyra¿on± w bajtach, 
    * a nie znakach. 
    * */
	uint32_t writeSstring(char* str,int16_t size=-1);
	uint32_t writeString(char* str, int64_t size=-1);
	
	uint32_t writeString(CharArray *ca);
	uint32_t writeSstring(CharArray *ca);

//======================= Zmiennoprzecinkowe =============================
	uint32_t writeDouble(double value);
	
	uint32_t writeBool(bool value);
	
	
 protected:
 	void appendStr(char* str, int64_t size);
 	
  	//int64_t htonll(int64_t src);

};
}
#endif /*PACKAGEBUFFER_H_*/
