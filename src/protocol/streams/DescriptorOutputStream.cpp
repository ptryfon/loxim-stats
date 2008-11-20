#include <unistd.h>
#include <protocol/streams/DescriptorOutputStream.h>

using namespace protocol;

DescriptorOutputStream::DescriptorOutputStream(int a_fd)
{
	fd=a_fd;
	status=OS_STATUS_OK;
}

void DescriptorOutputStream::close(void)
{	
	::close(fd);
	status=OS_STATUS_CLOSED;
}

int DescriptorOutputStream::write(char* buffor,unsigned long int off,unsigned long int length)
{		
	if (status<0) return status;
	ssize_t res=::write(fd,buffor+off,length);
	if (res<0)
	{
		status=OS_STATUS_OTHERERROR;
		return status;
	}
	return res;
}
