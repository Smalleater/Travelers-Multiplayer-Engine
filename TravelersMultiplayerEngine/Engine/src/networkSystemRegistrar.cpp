#include "networkSystemRegistrar.hpp"

#include "TME/engine/networkEcs.hpp"

#include "messageSystem.hpp"

namespace tme::engine
{
	void NetworkSystemRegistrar::registerNetworkSystems(NetworkEcs* _networkEcs)
	{
		// BeginUpdate

		// EndUpdate
		_networkEcs->registerEndUpdateSystem(std::make_shared<SendTcpMessageSystem>());
	}
}