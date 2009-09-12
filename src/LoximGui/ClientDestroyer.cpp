#include <Client/Client.h>
#include <LoximGui/ClientDestroyer.h>

using namespace std;
using namespace Util;

namespace LoximGui {
	
	ClientDestroyer::ClientDestroyer() : running(true)
	{
	}

	ClientDestroyer::~ClientDestroyer()
	{
		Mutex::Locker l(lock);
		running = false;
		cond.signal();
	}

	void ClientDestroyer::start()
	{
		while (true) {
			Queue q;
			{
				Mutex::Locker l(lock);
				if (queue.empty() && running)
					l.wait(cond);
				if (queue.empty() && !running)
					break;
				q.swap(queue);
			}
			for (Queue::iterator i = q.begin(); i != q.end(); ++i)
				delete *i;
		}
	}

	void ClientDestroyer::submit(std::auto_ptr<Client::Client> c)
	{
		Mutex::Locker l(lock);
		queue.push_back(c.release());
		cond.signal();
	}


}

