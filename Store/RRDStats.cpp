#include "RRDStats.h"

#include "rrd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

namespace Store {

	RRDStats::RRDStats(char *rrd, int step)
	{
		this->rrd = rrd;
		this->step = step;
		this->ec = new ErrorConsole("Store::RRDStats");

		::pthread_mutex_init(&rrdwriter.mutex, NULL);
		::pthread_cond_init(&rrdwriter.cond, NULL);
		rrdwriter.counter = 0;
		rrdwriter.lasttime = time(0) - 1;

		::pthread_create(&tid_rrdwriter, NULL, &rrdWriterThread, this);

		started = 1;
	}

	RRDStats::~RRDStats()
	{
		::pthread_mutex_lock(&rrdwriter.mutex);

		RRD_push(rrd, rrdwriter.counter, step);
		started = 0;

		::pthread_cond_signal(&rrdwriter.cond);
		::pthread_mutex_unlock(&rrdwriter.mutex);
		::pthread_join(tid_rrdwriter, NULL);
	}

	void* RRDStats::rrdWriterThread(void* arg)
	{
		RRDStats* r = (RRDStats*) arg;
		return r->rrdWriterMain();
	}

	void* RRDStats::rrdWriterMain(void)
	{
		ec->printf("watek rrd utworzony\n");

		for ( ; ; ) {
			::pthread_mutex_lock(&rrdwriter.mutex);

			while (rrdwriter.lasttime + step > time(0)) {
				::pthread_cond_wait(&rrdwriter.cond, &rrdwriter.mutex);
				if (! started)
					break;
			}

			rrdwriter.lasttime = time(0);

			if (started) {
				ec->printf("PUSH[%d]: %d\n", time(0), rrdwriter.counter);
				RRD_push(rrd, rrdwriter.counter, step);
				rrdwriter.counter = 0;
			} else {
				::pthread_mutex_unlock(&rrdwriter.mutex);
				break;
			}

			::pthread_mutex_unlock(&rrdwriter.mutex);
		}

		return NULL;
	}

	void RRDStats::RRD_inccounter(int incv)
	{
		::pthread_mutex_lock(&rrdwriter.mutex);
		rrdwriter.counter += incv;
		ec->printf("INC[%d]: %d", incv, rrdwriter.counter);
		::pthread_cond_signal(&rrdwriter.cond);
		::pthread_mutex_unlock(&rrdwriter.mutex);
	}

	int RRDStats::RRD_update(char *rrd, char *sum, unsigned int process_time)
	{
		char *argv[3];
		int   argc = 3;
		char val[128];

		sprintf(val, "%d:%s", process_time, sum);

		argv[0] = "rrdupdate";
		argv[1] = rrd;
		argv[2] = val;

		rrd_clear_error();
		rrd_update(argc, argv);

		return 0;
	}


	int RRDStats::RRD_create(char *rrd, char *sum, unsigned int process_time)
	{
		char *argv[128];
		int  argc=0;
		char s[16], start[64];
		char ds[64];

		argv[argc++] = "rrdcreate";
		argv[argc++] = rrd;
		argv[argc++] = "--step";
		sprintf(s, "%u", step);
		argv[argc++] = s;

		argv[argc++] = "--start";
		sprintf(start, "%u", process_time - 1);
		argv[argc++] = start;

		sprintf(ds, "DS:sum:ABSOLUTE:%d:0:1000", 4 * step);
		argv[argc++] = ds;

		argv[argc++] = "RRA:AVERAGE:0.5:1:360";
		argv[argc++] = "RRA:AVERAGE:0.5:4:360";
		argv[argc++] = "RRA:MAX:0.5:1:360";
		argv[argc++] = "RRA:MAX:0.5:4:360";

		rrd_clear_error();
		rrd_create(argc, argv);

		if(rrd_test_error()) {
			printf("RRD_create: %s\n", rrd_get_error());
			return 1;
		}

		return 0;
	}


	int RRDStats::RRD_push(char *rrd, char *sum, unsigned int step)
	{
		struct stat st;

		if (stat(rrd, &st)) {
			RRD_create(rrd, sum, time(0));
		}

		return RRD_update(rrd, sum, time(0));
	}

	int RRDStats::RRD_push(char *rrd, int sum, unsigned int step)
	{
		char ssum[12];
		sprintf (ssum, "%d", sum);

		return RRD_push(rrd, ssum, step);
	}
};
