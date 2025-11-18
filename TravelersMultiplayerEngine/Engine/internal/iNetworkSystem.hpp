#ifndef TME_ENGINE_I_NETWORK_SYSTEM_HPP
#define TME_ENGINE_I_NETWORK_SYSTEM_HPP

#include <cstdint>

namespace tme::engine
{
	using EntityId = uint64_t;

	class NetworkEcs;
	struct INetworkSystem
	{
		virtual ~INetworkSystem() = default;
		virtual void update(NetworkEcs* _ecs) = 0;
	};
}

#endif
