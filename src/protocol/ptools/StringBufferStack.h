#ifndef STRINGBUFFERSTACK_H_
#define STRINGBUFFERSTACK_H_

#include <stdint.h>


namespace protocol{


/**
 * Klasa pomocnicza dla StringBufferWriter. Oznacza pojedy�czy element stosu "napis�w", czyli fragment
 * bufora. W czasie konstrukcji bufora tworzymy stos takich struktur - tzn. w sytuacji w kt�rej do bie��cej 
 * g�owy stosu nie da si� doda� elementu - tworzony jest nowy obiekt tego typu.  
 */
class StringBufferStack
{
private:
	StringBufferStack *next;
	uint32_t buffer_len;
	//int used_size;
	uint32_t used_end; //Ile ��cznie bajt�w zapisali�my na koniec bufora.
	uint32_t used_start; //Ile ��cznie bajt�w zapisali�my na koniec bufora.
	char *buffer;
	
public:
	StringBufferStack(StringBufferStack *a_next,uint32_t a_length);
	
	virtual ~StringBufferStack();
	
	/**
	 * Pr�buje doda� wskazany "str" na pierwszym wolnym miejscu w tym buforze. Jak si� uda, 
	 * zwaca "true", jak nie - zwraca "false". 
	 */
	bool appendStr(char* str, uint32_t length);
	
	/**
	 * Pr�buje doda� wskazany "str" na ostatnim wolnym miejscu w tym buforze. Jak si� uda, 
	 * zwaca "true", jak nie - zwraca "false". 
	 */
	bool appendStrEnd(char* str, uint32_t length);
	
	char* getBuffer(){return buffer;};
	int getSize(){return used_start+used_end;};
	StringBufferStack* getNext(){return next;};
};
}
#endif /*STRINGBUFFERSTACK_H_*/
