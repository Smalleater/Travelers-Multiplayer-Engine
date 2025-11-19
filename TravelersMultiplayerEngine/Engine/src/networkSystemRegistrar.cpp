#include "networkSystemRegistrar.hpp"

#include "TME/engine/networkEcs.hpp"

#include "acceptConnectionSystem.hpp"
#include "messageSystem.hpp"

namespace tme::engine
{
	void NetworkSystemRegistrar::registerNetworkSystems(NetworkEcs* _networkEcs)
	{
		// BeginUpdate
		_networkEcs->registerBeginUpdateSystem(std::make_unique<AcceptConnectionSystem>());

		// EndUpdate
		_networkEcs->registerEndUpdateSystem(std::make_shared<SendTcpMessageSystem>());
	}
}