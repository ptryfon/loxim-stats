/**
 * StandardStat.h
 * @author: Pawel Tryfon (pt248444)
 *
 * The purpose of this file is to provide generic tool for gathering standard statistic data
 * such as minimum, maximum, average and standard deviation of something.
 */

#ifndef STANDARD_STAT_H
#define STANDARD_STAT_H

#include <time.h>
#include <sys/time.h>
#include <Errors/ErrorConsole.h>

namespace SystemStatsLib {

	/**
	 * StandardStat is a class gathering standard statistic data
	 * such as minimum, maximum, average and standard deviation of something.
	 */
	template <class T> class StandardStat {
		private:
			T sum;
			T val_min;
			T val_max;
			unsigned long n;
			T square_sum;
			time_t start_time;
			bool valid;
		public:
			StandardStat(): sum(0), val_min(0), val_max(0), n(0), square_sum(0), 
					start_time(time(NULL)), valid(false) {}
			virtual void reset()
			{
				sum = 0;
				val_max = 0;
				val_min = 0;
				n = 0;
				square_sum = 0;
				start_time = time(NULL);
				valid = false;
			}
			void add(const T val)
			{
			  	debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "StandardStat.add\n");
				if (valid) {
					sum += val;
					val_min = std::min(val_min, val);
					val_max = std::max(val_max, val);
					square_sum += val * val;
				}
				else {
					sum = val;
					val_min = val;
					val_max = val;
					square_sum = val * val;
					valid = true;
				}
				n++;
			}

			T get_min() const { return val_min; }
			T get_max() const { return val_max; }
			double get_avg() const { return (1.0 * sum)/n; } 
			double get_stddev() const
			{ 
				double avg = get_avg();
				return (1.0 * sum)/n - avg * avg;
			}
			double get_avg_in_time() const 
			{ 
				return 1.0 * sum/difftime(time(NULL), start_time);
			}
	};

	template <class T> class StandardStatContainer {
		private:
			StandardStat<T> global_stdstat;
		public:
			StandardStatContainer(): global_stdstat(StandardStat<T>()) {}
			virtual ~StandardStatContainer() {}
			virtual void reset() { global_stdstat.reset(); }
			virtual void add(const T val) { global_stdstat.add(val); }
			T get_max() const { return global_stdstat.get_max(); }
			T get_min() const { return global_stdstat.get_min(); }
			double get_avg() const { return global_stdstat.get_avg(); }
			double get_stddev() const { return global_stdstat.get_stddev(); }
			double get_avg_in_time() const { return global_stdstat.get_stddev(); }
	};

	template <class T> class StandardStatOnePeriodContainer: public StandardStatContainer<T> {
		private:
			StandardStat<T> local_stdstat;
		public:
			StandardStatOnePeriodContainer(): StandardStatContainer<T>(), 
				local_stdstat() {}
			virtual void reset() 
			{ 
				StandardStatContainer<T>::reset();
				local_stdstat.reset();
			}
			void local_reset() { local_stdstat.reset(); }
			virtual void add(const T val)
			{
				StandardStatContainer<T>::add(val);
				local_stdstat.add(val);
			}
			T get_local_max() const { return local_stdstat.get_max(); }
			T get_local_min() const { return local_stdstat.get_min(); }
			double get_local_avg() const { return local_stdstat.get_avg(); }
			double get_local_stddev() const { return local_stdstat.get_stddev(); }
			double get_local_avg_in_time() const { return local_stdstat.get_stddev(); }
	};

}

#endif //STANDARD_STAT_H

