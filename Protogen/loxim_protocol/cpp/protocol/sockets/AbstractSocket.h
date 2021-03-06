#ifndef ABSTRACTSOCKET_H_
#define ABSTRACTSOCKET_H_

#include "../streams/AbstractInputStream.h"
#include "../streams/AbstractOutputStream.h"

namespace protocol{

class AbstractSocket
{
	protected:
	
	public:
		virtual AbstractInputStream  *getInputStream()=0;	
	
		virtual AbstractOutputStream *getOutputStream()=0;
		
		virtual void close()=0;
		
		virtual ~AbstractSocket(){};
};
}
#endif /*ABSTRACTSOCKET_H_*/
