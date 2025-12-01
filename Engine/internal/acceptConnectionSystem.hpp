#ifndef TME_ENGINE_ACCEPT_CONNECTION_SYSTEM_HPP
#define TME_ENGINE_ACCEPT_CONNECTION_SYSTEN_HPP

#include "iNetworkSystem.hpp"

namespace tme::engine
{
	struct AcceptConnectionSystem : INetworkSystem
	{
		void update(NetworkEcs* _ecs) override;
	};
}

#endif
