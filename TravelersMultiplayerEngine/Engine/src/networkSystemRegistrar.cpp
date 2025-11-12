#include "networkSystemRegistrar.hpp"

#include "TME/engine/networkEcs.hpp"

#include "messageSystem.hpp"

namespace tme::engine
{
	void NetworkSystemRegistrar::registerNetworkSystems(NetworkEcs* _networkEcs)
	{
		std::shared_ptr<INetworkSystem> networkSystem;

		// BeginUpdate

		// EndUpdate
		networkSystem = std::make_shared<SendTcpMessageSystem>();
		_networkEcs->registerEndUpdateSystem(networkSystem);
	}
}