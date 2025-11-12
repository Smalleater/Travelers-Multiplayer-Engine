#include "TME/server/server.hpp"

#include <thread>
#include <chrono>

#include "TME/debugUtils.hpp"

#include "TME/engine/networkRootComponentTag.hpp"
#include "TME/engine/message.hpp"

using namespace tme;
using namespace tme::engine;
using namespace tme::server;


DECLARE_MESSAGE_BEGIN(HelloWorld)
FIELD(std::string, string)
DECLARE_MESSAGE_END()

#define TME

int main() {
	ErrorCode ec;

	ec = Server::Get()->Start(2025);
	if (ec != ErrorCode::Success) return -1;

	while (Server::Get()->isRunning())
	{
		Server::Get()->beginUpdate();

		std::vector<EntityId> entityNetworkRoot = Server::Get()->queryEntitiesWithComponent<engine::NetworkRootComponentTag>();
		for (size_t i = 0; i < entityNetworkRoot.size(); i++)
		{
			std::shared_ptr<HelloWorld> message = std::make_shared<HelloWorld>();
			message->string = "Hello World from server!";
			Server::Get()->sendTcpMessage(entityNetworkRoot[i], message);
		}

		Server::Get()->endUpdate();

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	ec = Server::Get()->Stop();
	if (ec != ErrorCode::Success) return -1;

	return 0;
}