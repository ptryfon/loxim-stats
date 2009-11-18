/*
 * AbstractStats.h
 *
 *  Created on: 18-Nov-09
 *      Author: Bartosz Borkowski
 */

#ifndef ABSTRACTSTATS_H_
#define ABSTRACTSTATS_H_

#include <string>

namespace SystemStatsLib{

	class AbstractStats {
		public:
			AbstractStats(const std::string& _name) : name(_name) {}
			const std::string& get_name() {return name;}
		private:
			std::string name;
	};

}

#endif /* ABSTRACTSTATS_H_ */
