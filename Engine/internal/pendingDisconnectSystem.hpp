#ifndef TME_ENGINE_PENDING_DISCONNECT_SYSTEM_HPP
#define TME_ENGINE_PENDING_DISCONNECT_SYSTEM_HPP

#include "iNetworkSystem.hpp"

namespace tme::engine
{
	struct PendingDisconnectSystem : INetworkSystem
	{
		void update(NetworkEcs* _ecs) override;
	};
}

#endif
