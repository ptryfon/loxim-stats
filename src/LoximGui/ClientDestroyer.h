#ifndef LOXIM_GUI_CLIENT_DESTROYER_H
#define LOXIM_GUI_CLIENT_DESTROYER_H

#include <memory>
#include <vector>
#include <Util/Thread.h>

namespace Client {
	class Client;
}

namespace LoximGui {
	
	class ClientDestroyer : public Util::Thread {
		public:
			typedef std::vector<Client::Client*> Queue;
			ClientDestroyer();
			~ClientDestroyer();
			void start();
			void submit(std::auto_ptr<Client::Client>);
		private:
			bool running;
			Queue queue;
			Util::Mutex lock;
			Util::CondVar cond;
	};
}

#endif /* LOXIM_GUI_CLIENT_DESTROYER_H */
