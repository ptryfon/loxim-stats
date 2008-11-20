#include <protocol/streams/DescriptorInputStream.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>


using namespace protocol;

DescriptorInputStream::DescriptorInputStream(int a_fd)
{
	fd=a_fd;
	status=IS_STATUS_OK;
}

void DescriptorInputStream::close(void)
{
	::close(fd);
	status=IS_STATUS_CLOSED;
}

int DescriptorInputStream::read(char* buffor, unsigned long int off,unsigned long int length)
{
	if (status<0 && status != IS_STATUS_TIMEDOUT)
		return status;
		
	ssize_t res=::read(fd,buffor+off,length);
	if (res<0)
	{
		//printf("!!!OTRZYMANO res: %d len: %d errno: %d %s!!!\n",res,length,errno,strerror(errno));
	    status=IS_STATUS_OTHERERROR;
	    return status;
	}else
	if ( res == 0)
	{
		status=IS_STATUS_EOF;
		return 0;
	}else{
		return res;
	};	
}



int DescriptorInputStream::read(char *buffer, unsigned long int off, unsigned long int length, sigset_t *sigmask, int *cancel)
{
        fd_set rfds;
	int res;
	sigset_t old_mask;
	if (status < 0 && status != IS_STATUS_TIMEDOUT)
		return status;
	
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
	pthread_sigmask(SIG_SETMASK, sigmask, &old_mask);
	if (*cancel){
		status = IS_STATUS_CANCEL;
		return status;
	}
	res = pselect(fd+1, &rfds, NULL, NULL, NULL, &old_mask);
	pthread_sigmask(SIG_SETMASK, &old_mask, NULL);
	if (res == 1){
		return read(buffer, off, length);
		
	} else {
		if (res == 0)
			status = IS_STATUS_TIMEDOUT;
		else 
			if (errno == EINTR && *cancel)
				status = IS_STATUS_CANCEL;
			else 
				status = IS_STATUS_OTHERERROR;
		return status;
	}
}
