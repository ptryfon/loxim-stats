#ifndef _MUTEX_
#define _MUTEX_

/*
 *	Julian Krzemi�ski (julian.krzeminski@students.mimuw.edu.pl)
 */
namespace SemaphoreLib {

class Mutex 
{
	private:
		pthread_mutex_t mutex;
	public:
		Mutex();
		virtual ~Mutex();

		virtual int init();
		virtual int init(pthread_mutexattr_t* attr);
		virtual int up();
		virtual int down();
};

class RecursiveMutex : public Mutex {
	private:
		pthread_mutexattr_t attr;
	public:
		virtual ~RecursiveMutex();
		
		virtual int init();
};

} /* namespace SemaphoreLib */

#endif
