#ifndef __STORE_RRDSTATS_H__
#define __STORE_RRDSTATS_H__

#include <pthread.h>

namespace Store
{
	class RRDStats;
};

#include "Store.h"
#include "Errors/ErrorConsole.h"

using namespace std;

namespace Store
{
	class RRDStats {

	private:
		char *rrd;
		int step;

		int started;

		pthread_t tid_rrdwriter;
		struct {
			pthread_mutex_t mutex;
			pthread_cond_t cond;
			int counter;
			int lasttime;
		} rrdwriter;

		ErrorConsole *ec;

		int RRD_update(char* rrd, char* sum, unsigned int process_time);
		int RRD_create(char* rrd, char* sum, unsigned int process_time);

	public:
		RRDStats(char *rrd, int step);
		~RRDStats();

		int RRD_push(char* rrd, char* sum, unsigned int step);
		int RRD_push(char* rrd, int sum, unsigned int step);
		void RRD_inccounter(int incc);

		static void* rrdWriterThread(void* arg);
		void* rrdWriterMain(void);
	};
};

#endif /* __STORE_RRDSTATS_H__ */
