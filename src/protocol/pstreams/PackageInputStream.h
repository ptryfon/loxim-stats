#ifndef PACKAGEINPUTSTREAM_H
#define PACKAGEINPUTSTREAM_H

#include <signal.h>
#include <protocol/ptools/Package.h>
#include <protocol/streams/AbstractInputStream.h>

namespace protocol{

/**
 *	Klasa na podstawie danego input stream'a zajmuje siê
 *  ukryciem warstwy bajtów - i umo¿liwia dzia³anie w sferze tylko i 
 *  wy³±cznie pakietów 
 * 
 * Uwaga - klasa - a w szczególno¶ci metoda readPackage() nie jest thread-safe.
 * Nale¿y zagwarantowaæ, ¿e jest wywo³ywana ona tylko przez jeden w±tek (lub wprowadziæ semafor
 * do tej klasy). 
 * 
 *  Wydaje siê, ¿e klasa ta jest u¿ywana wy³±cznie przez w±tek czytelnika dla danego po³±czenia jest s³uszne.
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
		 * Próbuje odczytaæ dok³adnie n-bajtów. W razie potrzeby ponawia odczyt fizyczny.
		 *  - Zwraca liczbê bajtów, któr± uda³o siê odczytaæ - oraz wype³nia buffor. 
		 */
		unsigned int readNbytes(char* buffor,unsigned long int n, sigset_t *sigmask, int *cancel);
};
}
#endif /*PACKAGEINPUTSTREAM_H*/
