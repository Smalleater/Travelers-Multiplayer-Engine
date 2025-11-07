#ifndef TME_ENGINE_PING_SYSTEM_HPP
#define TME_ENGINE_PING_SYSTEM_HPP

#include "iNetworkSystem.hpp"

namespace tme::engine
{
	struct PingSystem : public INetworkSystem
	{
		void update(NetworkEcs& _ecs) override;
	};
}

#endif
