#ifndef _MUTEX_
#define _MUTEX_

/*
 *	Julian Krzemiñski (julian.krzeminski@students.mimuw.edu.pl)
 */
namespace SemaphoreLib {

class Mutex 
{
	private:
		pthread_mutex_t mutex;
	public:
		Mutex();
		~Mutex();

		int init();
		int up();
		int down();
};

} /* namespace SemaphoreLib */

#endif
