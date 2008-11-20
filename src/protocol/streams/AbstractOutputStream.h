#ifndef ABSTRACTOUTPUTSTREAM_H
#define ABSTRACTOUTPUTSTREAM_H

#define OS_STATUS_OK			 0
#define OS_STATUS_CLOSED		-3
#define OS_STATUS_OTHERERROR	-4

namespace protocol{

class AbstractOutputStream
{	
  protected:
	
  public:
	virtual void close(void)=0;
	/**
	 * Zwraca status po zapisie
	 */
	virtual int write(char* buffor,unsigned long int off,unsigned long int length)=0;
	virtual int flush()=0;
		
	/**
	 * Zwraca status po zapisie
	 */
	virtual int write(char* buffor,unsigned long int length);
	
	virtual int getStatus()=0;
	
	virtual ~AbstractOutputStream(){};
};
}
#endif /*ABSTRACTOUTPUTSTREAM_H*/
