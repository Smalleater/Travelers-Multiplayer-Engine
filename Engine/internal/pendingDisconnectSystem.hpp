#ifndef TRA_ENGINE_PENDING_DISCONNECT_SYSTEM_HPP
#define TRA_ENGINE_PENDING_DISCONNECT_SYSTEM_HPP

#include "iNetworkSystem.hpp"

namespace tra::engine
{
	struct PendingDisconnectSystem : INetworkSystem
	{
		void update(NetworkEcs* _ecs) override;
	};
}

#endif
