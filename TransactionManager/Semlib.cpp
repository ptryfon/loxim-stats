#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "Semlib.h"

static void sem_call(int sem_id, int op)
{
    struct sembuf sb;
    sb.sem_num = 0;	// semaphore number
    sb.sem_op = op;	// semaphore operation
    sb.sem_flg = 0;     // semaphore flags
    if (semop(sem_id, &sb, 1) == -1)
    {
	printf("Error in semop\n");
	//exit(1);
    }
}

void P(int sem_id)
{
    sem_call(sem_id, -1);
}

void V(int sem_id)
{
    sem_call(sem_id, 1);
}  

  
