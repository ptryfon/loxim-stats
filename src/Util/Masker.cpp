#include <stdlib.h>
#include <Util/Masker.h>


namespace Util{
	Masker::Masker(const sigset_t &mask)
	{
		pthread_sigmask(SIG_SETMASK, &mask, &old_mask);
	}

	Masker::~Masker()
	{
		pthread_sigmask(SIG_SETMASK, &old_mask, NULL);
	}

	const sigset_t &Masker::get_old_mask()
	{
		return old_mask;
	}
}

