#include "TME/server/server.hpp"

#include "TME/debugUtils.hpp"

#include "TME/engine/networkRootComponentTag.hpp"

using namespace tme;
using namespace tme::server;

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
			TME_INFO_LOG("Entity with NetworkRootComponentTag: %s", std::to_string(entityNetworkRoot[i]).c_str());
		}

		Server::Get()->endUpdate();
	}

	ec = Server::Get()->Stop();
	if (ec != ErrorCode::Success) return -1;

	return 0;
}