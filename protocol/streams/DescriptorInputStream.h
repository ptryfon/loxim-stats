#ifndef  DESCRIPTOR_INPUT_STREAM_H
#define  DESCRIPTOR_INPUT_STREAM_H

#include "AbstractInputStream.h"

namespace protocol{

class DescriptorInputStream: public AbstractInputStream
{
	private:
		int fd;
		int status;
		
	public:
		DescriptorInputStream(int a_fd);
		
		virtual void close(void);
		
		virtual int read(char* buffor, unsigned long int off, unsigned long int length);
		virtual int read(char* buffor, unsigned long int off, unsigned long int length, sigset_t *sigmask, int *cancel);	
			
		virtual int getStatus() {return status;};
};
}
#endif /*DESCRIPTOR_INPUT_STREAM_H*/
