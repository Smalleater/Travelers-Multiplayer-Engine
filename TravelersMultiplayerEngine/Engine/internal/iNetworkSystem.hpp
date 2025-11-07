#ifndef TME_ENGINE_I_NETWORK_SYSTEM_HPP
#define TME_ENGINE_I_NETWORK_SYSTEM_HPP

namespace tme::engine
{
	class NetworkEcs;
	struct INetworkSystem
	{
		virtual ~INetworkSystem() = default;
		virtual void update(NetworkEcs& _ecs) = 0;
	};
}

#endif
