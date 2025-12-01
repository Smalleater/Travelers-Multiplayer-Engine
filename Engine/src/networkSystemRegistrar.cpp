#include "networkSystemRegistrar.hpp"

#include "TME/engine/networkEcs.hpp"

#include "acceptConnectionSystem.hpp"
#include "messageSystem.hpp"
#include "pendingDisconnectSystem.hpp"
#include "disconnectSystem.hpp"

namespace tme::engine
{
	void NetworkSystemRegistrar::registerNetworkSystems(NetworkEcs* _networkEcs)
	{
		// BeginUpdate
		_networkEcs->registerBeginUpdateSystem(std::make_unique<DisconnectSystem>());
		_networkEcs->registerBeginUpdateSystem(std::make_unique<PendingDisconnectSystem>());
		_networkEcs->registerBeginUpdateSystem(std::make_unique<AcceptConnectionSystem>());
		_networkEcs->registerBeginUpdateSystem(std::make_unique<ReceiveTcpMessageSystem>());

		// EndUpdate
		_networkEcs->registerEndUpdateSystem(std::make_shared<SendTcpMessageSystem>());
	}
}