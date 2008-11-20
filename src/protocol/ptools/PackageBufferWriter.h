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
	 *  Stosu bufor�w. W g�owie stosu znajduje si� ostatnio dodany "podbufor". 
	 */
	StringBufferStack *stringBufferHead;
	
	/**
	 * Aktualna ilo�� wszystkich wpisanych znak�w.  
	 */
	int size; 


public:
	PackageBufferWriter();
	virtual ~PackageBufferWriter();
	
	/**
	 * Pobiera aktualny bufor utworzony przez klas� (jako wska�nik
	 * do pami�ci zawartej w klasie). Automatycznie wywo�uje flush. 
	 */
	const char* getBuffer();
	
	const char* getPackageBuffer(uint8_t packageType);
	
	/**
	 * Zwraca aktualn� d�ugo�� (w bajtach) utworzonego bufora. 
	 * Zwr�cona warto�� nie obejmuje 5 bajtowego nag��wka utworzonego
	 * na pocz�tku bufora - wi�c musisz te 5 bajt�w doliczy� r�cznie.
	 */
	 int getSize(); 
	
	/**
	 * Powyduje uporz�dkowanie wszystkich fragment�w
	 * i uformowanie ich w bufor wynikowy, a nast�pnie
	 * zwolnienie pomi�ci z tych fragment�w pomocniczych.  
	 * */
	void flush();
	
	/**
	 * Czy�ci zupe�nie bufor i wszystkie zmienne pomocnicze.
	 * W praktyce oznacza rozpocz�cie przygotowywania nowej paczki.
	 */
	void clear();


//================= TYPY CA�KOWITOLICZBOWE =============================
	/**
	 * Zwraca liczb� bajt�w zapisanych w buforze
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
   
   /*Je�li size<0 to badamy d�ugo�� �a�cucha str. 
    * size powinien by� d�ugo�ci� napisu wyra�on� w bajtach, 
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
