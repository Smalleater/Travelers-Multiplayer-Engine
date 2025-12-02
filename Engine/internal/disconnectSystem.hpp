#ifndef TRA_ENGINE_DISCONNECT_SYSTEM_HPP
#define TRA_ENGINE_DISCONNECT_SYSTEM_HPP

#include "iNetworkSystem.hpp"

namespace tra::engine
{
	struct DisconnectSystem : INetworkSystem
	{
		void update(NetworkEcs* _ecs) override;
	};
}

#endif
