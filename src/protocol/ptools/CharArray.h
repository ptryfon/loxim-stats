#ifndef CHARARRAY_H_
#define CHARARRAY_H_

#include <stdint.h>

namespace protocol{

class CharArray
{
	private:
		 char* buffor;
		 uint32_t size;
	
	public:		
		/**
		 * Tworzy CharArray "ukrywaj±cy" dan± tablicê znaków
		 * Dany bufor nie jest powielany, a w destruktorze obiektu jest zwalniany. 
		 */
		CharArray(char* a_buffor, uint32_t a_size);
		
		/**
		 * Tworzy CharArray "ukrywaj±cy" dany ci±g znaków zakoñczony znakiem pustym.
		 * Znak pusty nie jest zapisywany w buforze (tzn. d³ugo¶æ jest podawana bez znaku 0)
		 * 
		 * Dany bufor nie jest powielany, a w destruktorze obiektu jest zwalniany. 
		 */
		CharArray(char* a_buffor);
		
		virtual ~CharArray();
		
		char* getBuffor(){return buffor;};		
		uint32_t getSize(){return size; };
		
		bool equals(CharArray *ca);
		
		static bool equal(CharArray *a1, CharArray *a2);	
};
}
#endif /*CHARARRAY_H_*/
