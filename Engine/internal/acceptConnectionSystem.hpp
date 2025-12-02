#ifndef TRA_ENGINE_ACCEPT_CONNECTION_SYSTEM_HPP
#define TRA_ENGINE_ACCEPT_CONNECTION_SYSTEN_HPP

#include "iNetworkSystem.hpp"

namespace tra::engine
{
	struct AcceptConnectionSystem : INetworkSystem
	{
		void update(NetworkEcs* _ecs) override;
	};
}

#endif
