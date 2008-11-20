#ifndef STATEMENTSIMPLEPARSER_
#define STATEMENTSIMPLEPARSER_

#define 	STACK_SIZE  	1024

namespace LoximClient{

    /**
     * Klasa do�� wolna - ale do przetwarzania polece� wk�adanych
     * przez u�ytkownika - r�cznie - wystarczaj�co.
     * 
     * Wolna jest przedewszystkim operacja append - kt�ra realokuje bufor,
     * co mo�na zast�pi� stosem bufor�w.   
     */
    class StatementSimpleParser
    {
	private:
		char* buffer;
		
	public:
		StatementSimpleParser();
		~StatementSimpleParser();
		void append(char* str);
		
		/**
		 * 	S� dwie mo�liwo�ci:
		 * 		a) Funkcja zwraca pe�ny statement (do zwolnienia przez u�ytkownika za pomoc� free)
		 * 		b) Funkcja zwraca NULL (bo statementu nie znaleziono) i je�li for_stack!=NULL to 
		 * 		wpisuje na niego symbol, kt�ry le�y na szczycie stosu lexera - do wypisania w prompcie;
		 * 		Je�li stos jest pusty - to wpisuje 0.
		 */
		char *getStatement(char* for_stack);
		
    };
}

#endif /*STATEMENTSIMPLEPARSER_*/
