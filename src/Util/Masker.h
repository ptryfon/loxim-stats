#ifndef MASKER_H
#define MASKER_H

#include <signal.h>

namespace Util{
	class Masker{
		private:
			sigset_t old_mask;
		public:
			Masker(const sigset_t &);
			const sigset_t &get_old_mask();
			~Masker();
	};
}

#endif /* MASKER_H */
