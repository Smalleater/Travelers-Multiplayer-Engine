#ifndef TME_ENGINE_PING_COMPONENT_HPP
#define TME_ENGINE_PING_COMPONENT_HPP

#include <cstdint>

#include "TME/engine/iNetworkComponent.hpp"

namespace tme::engine
{
	struct PingComponent : public INetworkComponent
	{
		uint32_t m_latencyMs = 0;
	};

	struct RecvPingComponentTag : public INetworkComponent
	{

	};
}

#endif