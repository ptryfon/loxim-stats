#ifndef __MONITOR_H__
#define __MONITOR_H__

#include <pthread.h>
#include <Errors/Errors.h>	


namespace Indexes {
	

	class Monitor {
		private:
			pthread_mutex_t mutex;
			pthread_cond_t cond;
			
		public:
			Monitor();
			~Monitor();
			int lock();
			int unlock();
			int wait();
			int signal();
	};
	
}

#endif /*__MONITOR_H__*/
