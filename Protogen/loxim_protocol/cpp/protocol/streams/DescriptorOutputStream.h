#ifndef DESCRIPTOROUTPUTSTREAM_H
#define DESCRIPTOROUTPUTSTREAM_H

#include "AbstractOutputStream.h"

namespace protocol{

class DescriptorOutputStream: public AbstractOutputStream
{	
  private:
  	int fd;
  	int status;
  	
  protected:
	virtual ~DescriptorOutputStream(){};
	
  public:
  	DescriptorOutputStream(int a_fd);
  
	virtual void close(void);
	
	/**
	 * Zwraca status po zapisie
	 */
	virtual int write(char* buffor,unsigned long int off,unsigned long int length);
	
	virtual int flush(){return status;};	
	int getStatus(){return status;};

};
}
#endif /*DESCRIPTOROUTPUTSTREAM_H*/
