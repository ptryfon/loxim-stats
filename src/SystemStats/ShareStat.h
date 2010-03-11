/**
 * ShareStat.h
 * @author: Pawe³ Tryfon (pt248444)
 *
 * The purpose of this file is to provide generic tool for gathering statistic data
 * about the shares of particular events in the whole
 */

#ifndef SHARESTAT_H
#define SHARESTAT_H

#include <vector>

namespace SystemStatsLib {
	
	/**
	 * ShareStat is a class gathering statistic data about events named by enum type Enum.
	 * Enum values should be continous.
	 * T should be any integer type
	 */
	template <class Enum, class T> class ShareStat{
	
		private:
			T sum;
			std::vector<T> counters;

		public:
			ShareStat() {}
			explicit ShareStat(const unsigned int enum_size):sum(0), counters(enum_size + 1, 0) {}
			void reset() 
			{ 
				sum = 0;
				counters.assign(counters.size(), 0);
			}					
			void add(const Enum e, const T val) { counters[e] += val; }
			T get(const Enum e) const { return counters[e]; }
			double get_share(const Enum e) const
			{
				return sum != 0 ? 100*static_cast<double>(counters[e])/sum : 0;
			}
	};

	/**
	 * ShareStatContainer is a container class for ShareStat. The purpose of this class
	 * is gathering statistics from the start of statistic gathering
	 */
	template <class Enum, class T> class ShareStatContainer {
	
		public:
			explicit ShareStatContainer(const unsigned int enum_size):
				global_sharestat(enum_size) {}
			ShareStatContainer(const ShareStatContainer& ssc)
			{
				this->global_sharestat = ssc.global_sharestat;
			}
			virtual ~ShareStatContainer() {}
			virtual void reset() 
			{ 
				global_sharestat.reset(); 
			}
			virtual void add (const Enum e, const T val) 
			{ 
				global_sharestat.add(e,val);
			}
			T get_global(const Enum e) const {return global_sharestat.get(e); }
			double get_global_share(const Enum e) const 
			{
				return global_sharestat.get_share(e); 
			}
		private:
			ShareStat<Enum, T> global_sharestat;
	};

	/** 
	 * ShareStatOnePeriodContainer extends ShareStatContainer class with gathering statistics for some last period of time (locally). 
	 */ 
	template <class Enum, class T> class ShareStatOnePeriodContainer: public ShareStatContainer<Enum, T> {
	
		public:
			explicit ShareStatOnePeriodContainer(const unsigned int enum_size): ShareStatContainer<Enum, T>(enum_size),
				local_sharestat(enum_size) {}
				
			virtual void reset() 
			{ 
				ShareStatContainer<Enum, T>::reset();
				local_sharestat.reset(); 
			}
			void local_reset() { local_sharestat.reset(); }
			virtual void add (const Enum e, const T val) 
			{ 
				ShareStatContainer<Enum, T>::add(e, val);
				local_sharestat.add(e,val);
			}
			T get_local(const Enum e) const {return local_sharestat.get(e); }
			double get_local_share(const Enum e) const
			{
				return local_sharestat.get_share(e);
			}
		private:
			ShareStat<Enum, T> local_sharestat;
	};
}

#endif // SHARESTAT_H
