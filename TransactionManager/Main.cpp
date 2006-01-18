#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "RWUJSemaphore.h"

using namespace SemaphoreLib;

void* read( void*);
void* write(void*);

Semaphore* sem;

int main()
{
	sem = new RWUJSemaphore();	
	sem->init();
    
	int ith = 50;
	pthread_t threads[ith];
	int j = 10;
	for (int i=0; i<ith; i++)
	{
	    if (i<j)
		pthread_create(&threads[i], NULL, &write, &i);
	    else if (i > 15 && i < 19)
		pthread_create(&threads[i], NULL, &write, &i);
	    else
		pthread_create(&threads[i], NULL, &read,  &i);
	}
	
	for (int i=0; i<ith; i++)
	{
	    pthread_join(threads[i], NULL);
	}
	
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
