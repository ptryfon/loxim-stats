#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Util/Concurrency.h>

using namespace Util;

void* read( void*);
void* upgrade( void*);
void* write(void*);

RWUJSemaphore* sem;

#define MAX 100

int main()
{
	int ith = 1;
	pthread_t threads[MAX];
	int arg[MAX];
        int test = 2;		/* TEST MODE = 1 or 2 */

	sem = new RWUJSemaphore();
    
	if (test == 1)
	{
	    ith = 50;
	    int j = 10;
	    for (int i=0; i<ith; i++)
	    {
		arg[i] = i;
		if (i<j)
		    pthread_create(&threads[i], NULL, &write, &arg[i]);
		else if (i > 15 && i < 19)
		    pthread_create(&threads[i], NULL, &write, &arg[i]);
		else
		    pthread_create(&threads[i], NULL, &read,  &arg[i]);
	    }
	}
	else if (test == 2)
	{
	    ith = 6;
	    for (int i=0; i<ith; i++)
	    {
		arg[i] = i;
		if (i < 2)
    		    pthread_create(&threads[i], NULL, &read, &arg[i]);
		else if (i  < 5)
		    pthread_create(&threads[i], NULL, &upgrade, &arg[i]);
		else
		    pthread_create(&threads[i], NULL, &write, &arg[i]);
	    }
	}
	
	for (int i=0; i<ith; i++)
	{
	    pthread_join(threads[i], NULL);
	}
	//sem->status();
	delete sem;
	return 0;
}

void* read(void* v)
{
    int id = *(int*) v;
    sem->lock_read();
    printf("Reader %d inside\n", id); fflush(stdout);    
    if (id % 2 == 0 ) usleep(2);
    for (int i=0; i<1000; i++) ;
    sem->unlock();
    printf("Reader %d out\n", id); fflush(stdout);	
    return NULL;
}

void* upgrade(void* v)
{
    bool stat;
    int id = *(int*) v;
    sem->lock_read();
    printf("Upgrader %d inside\n", id); fflush(stdout);
    usleep(3);
    for (int i=0; i<1000; i++) ;
    stat = sem->lock_upgrade(id);
    printf("Upgrader %d %supgraded\n", id, stat?"":"not "); fflush(stdout);
    //sem->status();
    if  (stat)
    {
	sem->unlock();
	printf("Upgrader %d out\n", id); fflush(stdout);
    }
    else
    {
	printf("Upgrader %d aborted on upgrading\n", id); fflush(stdout);
	//sem->status();
    }
    return NULL;
}

void* write(void* v)
{
    int id = *(int*) v;
    sem->lock_write();
    printf("Writer %d inside\n", id); fflush(stdout);
    for (int i=0; i<1000; i++) ;
    sem->unlock();
    printf("Writer %d out\n", id); fflush(stdout);
    return NULL;
}
