#ifndef TME_ENGINE_MESSAGE_SYSTEM_HPP
#define TME_ENGINE_MESSAGE_SYSTEM_HPP

#include "iNetworkSystem.hpp"

namespace tme::engine
{
	struct SendTcpMessageSystem : public INetworkSystem
	{
		void update(NetworkEcs& _ecs) override;
	};
}

#endif
