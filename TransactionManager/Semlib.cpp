#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "Semlib.h"

int create_sem(int sem_id)
{
    return (semget(sem_id, 1, 0666 | IPC_CREAT | IPC_EXCL));
}

int release_sem(int sem_id)
{
    return (semctl (sem_id, 0, IPC_RMID,0));
}

static int sem_call(int sem_id, int op)
{
    struct sembuf sb;
    sb.sem_num = 0;		// semaphore number
    sb.sem_op = op;		// semaphore operation
    sb.sem_flg = 0;     // semaphore flags
    return semop(sem_id, &sb, 1);
}

int P(int sem_id)
{
    return sem_call(sem_id, -1);
}

int V(int sem_id)
{
	return sem_call(sem_id, 1);
}