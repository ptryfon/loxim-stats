#ifndef STATEMENTSIMPLEPARSER_
#define STATEMENTSIMPLEPARSER_

#define 	STACK_SIZE  	1024

namespace LoximClient{

    /**
     * Klasa do¶æ wolna - ale do przetwarzania poleceñ wk³adanych
     * przez u¿ytkownika - rêcznie - wystarczaj±co.
     * 
     * Wolna jest przedewszystkim operacja append - która realokuje bufor,
     * co mo¿na zast±piæ stosem buforów.   
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
		 * 	S± dwie mo¿liwo¶ci:
		 * 		a) Funkcja zwraca pe³ny statement (do zwolnienia przez u¿ytkownika za pomoc± free)
		 * 		b) Funkcja zwraca NULL (bo statementu nie znaleziono) i je¶li for_stack!=NULL to 
		 * 		wpisuje na niego symbol, który le¿y na szczycie stosu lexera - do wypisania w prompcie;
		 * 		Je¶li stos jest pusty - to wpisuje 0.
		 */
		char *getStatement(char* for_stack);
		
    };
}

#endif /*STATEMENTSIMPLEPARSER_*/
