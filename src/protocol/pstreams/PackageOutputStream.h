#ifndef PACKAGEOUTPUTSTREAM_H
#define PACKAGEOUTPUTSTREAM_H

#include <protocol/ptools/Package.h>
#include <protocol/streams/PriorityOutputStream.h>

namespace protocol{
/**
 *	Klasa na podstawie danego outout stream'a zajmuje si�
 *  ukryciem warstwy bajt�w - i umo�liwia �ycie w sferze tylko i 
 *  wy��cznie pakiet�w.
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
		 * 0 -> operacja zako�czona sukcesem
		 */
		int writePackage(Package *package);
		
		/*
		 * 0 -> operacja zako�czona sukcesem
		 */
		int writePackagePriority(Package *package);
};
}
#endif /*PACKAGEOUTPUTSTREAM_H*/
