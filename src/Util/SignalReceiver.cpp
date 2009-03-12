#include <Util/SignalReceiver.h>
#include <Util/SignalRouter.h>

namespace Util {
	SignalReceiver::~SignalReceiver()
	{
		SignalRouter::unregister_thread(thread);
	}

}

