#include "TME/server/server.hpp"

#include <thread>
#include <chrono>

#include "TME/debugUtils.hpp"

#include "TME/engine/networkRootComponentTag.hpp"
#include "TME/engine/message.hpp"
#include "TME/engine/newConnectionComponent.hpp"
#include "TME/engine/connectionStatusComponent.hpp"

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

	EntityId selfEntityId = Server::Get()->getSelfEntityId();

	while (Server::Get()->isRunning())
	{
		Server::Get()->beginUpdate();

		std::vector<EntityId> queryIds = Server::Get()->queryEntityIds<NetworkRootComponentTag, ConnectedComponentTag>();
		for (size_t i = 0; i < queryIds.size(); i++)
		{
			if (queryIds[i] == selfEntityId) continue;

			std::shared_ptr<HelloWorld> message = std::make_shared<HelloWorld>();
			message->string = "Hello World from server!";
			Server::Get()->sendTcpMessage(queryIds[i], message);
		}

		Server::Get()->endUpdate();

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	/*ec = Server::Get()->Stop();
	if (ec != ErrorCode::Success) return -1;*/

	return 0;
}