#ifndef PACKAGEINPUTSTREAM_H
#define PACKAGEINPUTSTREAM_H

#include <signal.h>
#include <protocol/ptools/Package.h>
#include <protocol/streams/AbstractInputStream.h>

namespace protocol{

/**
 *	Klasa na podstawie danego input stream'a zajmuje si�
 *  ukryciem warstwy bajt�w - i umo�liwia dzia�anie w sferze tylko i 
 *  wy��cznie pakiet�w 
 * 
 * Uwaga - klasa - a w szczeg�lno�ci metoda readPackage() nie jest thread-safe.
 * Nale�y zagwarantowa�, �e jest wywo�ywana ona tylko przez jeden w�tek (lub wprowadzi� semafor
 * do tej klasy). 
 * 
 *  Wydaje si�, �e klasa ta jest u�ywana wy��cznie przez w�tek czytelnika dla danego po��czenia jest s�uszne.
 * 
 * @author Piotr Tabor
 */
class PackageInputStream
{
	private:
		AbstractInputStream *inputStream;
	
	public:
		PackageInputStream(AbstractInputStream *a_inputStream);
		virtual ~PackageInputStream();
	
		Package *readPackage();
		Package *readPackage(sigset_t *sigmask, int *cancel);

		int getStatus();
	private:
		/**
		 * Pr�buje odczyta� dok�adnie n-bajt�w. W razie potrzeby ponawia odczyt fizyczny.
		 *  - Zwraca liczb� bajt�w, kt�r� uda�o si� odczyta� - oraz wype�nia buffor. 
		 */
		unsigned int readNbytes(char* buffor,unsigned long int n, sigset_t *sigmask, int *cancel);
};
}
#endif /*PACKAGEINPUTSTREAM_H*/
