#ifndef _SEMLIB_
#define _SEMLIB_

#define SEMKEY1	((key_t) 7892)	/* mutex for creating transactions */

int release_sem(int sem_id);
int create_sem(int sem_id);
void P(int sem_id);
void V(int sem_id);

#endif
