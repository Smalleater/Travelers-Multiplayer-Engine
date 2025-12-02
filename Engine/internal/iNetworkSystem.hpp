#ifndef TRA_ENGINE_I_NETWORK_SYSTEM_HPP
#define TRA_ENGINE_I_NETWORK_SYSTEM_HPP

#include <cstdint>

namespace tra::engine
{
	using EntityId = uint32_t;

	class NetworkEcs;
	struct INetworkSystem
	{
		virtual ~INetworkSystem() = default;
		virtual void update(NetworkEcs* _ecs) = 0;
	};
}

#endif
