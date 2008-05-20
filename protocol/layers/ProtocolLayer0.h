#ifndef PROTOCOLLAYER0_H
#define PROTOCOLLAYER0_H

#include "../ptools/Package.h"
#include "../pstreams/PackageInputStream.h"
#include "../pstreams/PackageOutputStream.h"
#include "../sockets/AbstractSocket.h"
#include "../streams/PriorityOutputStream.h"

namespace protocol{

	/**
	 *	Klasa na podstawie danego input i outout streama (wzgl�dnie socket'a) 
	 *  zajmuje si� obs�ug� komunikacji w warstwie pakiet�w. 
	 * 
	 *  Klasa ta interpetuje pakiety:
	 * 	<OL>
	 * 		<LI>ASCPing - poprzez odes�anie odpowiedzi pong</LI>
	 * 	</OL>  
	 * 
	 * 	Interpretacja tego typu jest zar�wno prawid�owa dla aplikacji pracuj�cych po
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
			Package *readPackage(long timeout);
			int getLastError();
			bool writePackage(Package *package);
			bool writePackagePriority(Package *package);

		protected:
			int lastError;
			virtual bool interpretePackage(Package *package);		
	};

}

#endif /*PROTOCOLLAYER0_H*/
