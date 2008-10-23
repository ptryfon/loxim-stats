#ifndef ABSTRACTINPUTSTREAM_H
#define ABSTRACTINPUTSTREAM_H

#define IS_STATUS_OK			 0
#define IS_STATUS_EOF			-1
#define IS_STATUS_CAN_NOT_OPEN		-2
#define IS_STATUS_CLOSED		-3
#define IS_STATUS_TIMEDOUT		-4
#define IS_STATUS_OTHERERROR		-5

namespace protocol{

class AbstractInputStream
{	
  protected:
	
  public:
	virtual void close(void)=0;
	virtual int read(char* buffor,unsigned long int off,unsigned long int length)=0;
	
	virtual int read(char* buffor, unsigned long int length);
	virtual int read(char* buffor, unsigned long int off, unsigned long int length, long timout)=0;
	virtual int getStatus()=0;
	
	virtual ~AbstractInputStream(){};
};
}
#endif /*ABSTRACTINPUTSTREAM_H*/
