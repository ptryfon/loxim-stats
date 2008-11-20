#ifndef PACKAGEOUTPUTSTREAM_H
#define PACKAGEOUTPUTSTREAM_H

#include <protocol/ptools/Package.h>
#include <protocol/streams/PriorityOutputStream.h>

namespace protocol{
/**
 *	Klasa na podstawie danego outout stream'a zajmuje siê
 *  ukryciem warstwy bajtów - i umo¿liwia ¿ycie w sferze tylko i 
 *  wy³±cznie pakietów.
 * 
 */
class PackageOutputStream
{
	private:
		PriorityOutputStream *outputStream;
	
	public:
		PackageOutputStream(PriorityOutputStream *a_outputStream);
		virtual ~PackageOutputStream();

		/*
		 * 0 -> operacja zakoñczona sukcesem
		 */
		int writePackage(Package *package);
		
		/*
		 * 0 -> operacja zakoñczona sukcesem
		 */
		int writePackagePriority(Package *package);
};
}
#endif /*PACKAGEOUTPUTSTREAM_H*/
