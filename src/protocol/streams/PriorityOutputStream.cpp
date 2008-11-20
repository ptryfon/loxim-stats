#include <protocol/streams/PriorityOutputStream.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h> 

using namespace protocol;

PriorityOutputStream::PriorityOutputStream(AbstractOutputStream *a_baseOutputStream)
{
	//TODO: Obs³uga b³êdów
	semaphores=::semget(IPC_PRIVATE, 2, IPC_CREAT|0600);
    ::semctl( semaphores, 0, SETVAL, 1);
    ::semctl( semaphores, 1, SETVAL, 1);
	baseOutputStream=a_baseOutputStream;
}

PriorityOutputStream::~PriorityOutputStream()
{
	::semctl(semaphores, 0, IPC_RMID);
}
  
int PriorityOutputStream::write(char* buffor,unsigned long int off,unsigned long int length)
{
		struct sembuf SEM_OP_P0P1V1[3]; //równoczesne opuszczenie 0 i 1, a potem podniesienie 1
		struct sembuf SEM_OP_V0; //podniesienie 0
		SEM_OP_P0P1V1[0].sem_num=0;
		SEM_OP_P0P1V1[0].sem_op=-1;
		SEM_OP_P0P1V1[0].sem_flg=0;
		SEM_OP_P0P1V1[1].sem_num=1;
		SEM_OP_P0P1V1[1].sem_op=-1;
		SEM_OP_P0P1V1[1].sem_flg=0;
		SEM_OP_P0P1V1[2].sem_num=1;
		SEM_OP_P0P1V1[2].sem_op=+1;
		SEM_OP_P0P1V1[2].sem_flg=0;
		
		SEM_OP_V0.sem_num=0;
		SEM_OP_V0.sem_op=+1;
		SEM_OP_V0.sem_flg=0;
		
		if ( ::semop(semaphores,SEM_OP_P0P1V1,3) == 0 )
		{
			
			int res=baseOutputStream->write(buffor,off,length);			
			if (semop(semaphores,&SEM_OP_V0,1)!=0)
			{
				//TODO: Obs³uga b³êdu
				return OS_STATUS_OTHERERROR;
			};
			return res;
		}else
			return OS_STATUS_OTHERERROR;		
}

int PriorityOutputStream::writePriority(char* buffor,unsigned long int off,unsigned long int length)
{
		struct sembuf SEM_OP_P1V1[2]; //równoczesne opuszczenie i podniesienie 1
		
		struct sembuf SEM_OP_P0; //podniesienie 0
		struct sembuf SEM_OP_V0; //opuszczenie 0

		SEM_OP_P0.sem_num=0;
		SEM_OP_P0.sem_op=-1;
		SEM_OP_P0.sem_flg=0;
		
		SEM_OP_V0.sem_num=0;
		SEM_OP_V0.sem_op=+1;
		SEM_OP_V0.sem_flg=0;
		
		SEM_OP_P1V1[0].sem_num=1;
		SEM_OP_P1V1[0].sem_op=-1;
		SEM_OP_P1V1[0].sem_flg=0;
		
		SEM_OP_P1V1[1].sem_num=1;
		SEM_OP_P1V1[1].sem_op=+1;
		SEM_OP_P1V1[1].sem_flg=0;
		
		if ( ::semop(semaphores,&SEM_OP_P0,1) == 0 )
		{
			if ( ::semop(semaphores,SEM_OP_P1V1,2) == 0 )
			{
				
				int res=baseOutputStream->write(buffor,off,length);			
				if (semop(semaphores,&SEM_OP_V0,1)!=0)
				{
					//TODO: Obs³uga b³êdu
					return OS_STATUS_OTHERERROR;
				};
				return res;
			}else
				return OS_STATUS_OTHERERROR;		
		}else
				return OS_STATUS_OTHERERROR;
}
	
		
void PriorityOutputStream::close(void)
{ 
	baseOutputStream->close();
}
