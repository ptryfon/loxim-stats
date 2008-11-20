#ifndef PROTOCOLLAYER0_H
#define PROTOCOLLAYER0_H

#include "../ptools/Package.h"
#include "../pstreams/PackageInputStream.h"
#include "../pstreams/PackageOutputStream.h"
#include "../sockets/AbstractSocket.h"
#include "../streams/PriorityOutputStream.h"

namespace protocol{

	/**
	 *	Klasa na podstawie danego input i outout streama (wzglêdnie socket'a) 
	 *  zajmuje siê obs³ug± komunikacji w warstwie pakietów. 
	 * 
	 *  Klasa ta interpetuje pakiety:
	 * 	<OL>
	 * 		<LI>ASCPing - poprzez odes³anie odpowiedzi pong</LI>
	 * 	</OL>  
	 * 
	 * 	Interpretacja tego typu jest zarówno prawid³owa dla aplikacji pracuj±cych po
	 *  stronie klienta - jak i po stronie serwera. 
	 */ 
	class ProtocolLayer0
	{
		private:
			PackageInputStream *inputStream;
			PackageOutputStream *outputStream;
			PriorityOutputStream *priorityOutputStream;

		public:
			ProtocolLayer0(AbstractInputStream *a_inputStream, AbstractOutputStream *a_outputStream);
			ProtocolLayer0(AbstractSocket *a_socket);
			virtual ~ProtocolLayer0();

			Package *readPackage();

			/** 
			 * read a package until successful completion or cancel
			 * turns into true. To avoid race conditions, sometimes
			 * asynchronous changes of cancel should be blocked. It
			 * is assumed that it will be changed only by signal
			 * handlers, so blocking is done through setting the
			 * sigmask. It is the callers duty to take care that
			 * sigmask is proper, that is, that setting it would
			 * really block the handlers which touch cancel.
			 */
			Package *readPackage(sigset_t *sigmask, int *cancel);
			int getLastError();
			bool writePackage(Package *package);
			bool writePackagePriority(Package *package);

		protected:
			int lastError;
			virtual bool interpretePackage(Package *package);		
	};

}

#endif /*PROTOCOLLAYER0_H*/
