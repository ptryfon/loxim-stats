#ifndef _SEMAPHORE_
#define _SEMAPHORE_

namespace SemaphoreLib {

class Semaphore
{
	public:
		Semaphore();
		virtual ~Semaphore();

		/* abstract methods 	*/
		virtual int init() 	 = 0;
		virtual int destroy() 	 = 0;
		virtual int lock_read()	 = 0;
		virtual	int lock_write() = 0;
		virtual int unlock()	 = 0;
		virtual int lock_exchange()   = 0;
};

} /* namespace SemaphoreLib  */
#endif
