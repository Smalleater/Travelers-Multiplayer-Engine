#ifndef TRA_ENGINE_MESSAGE_SYSTEM_HPP
#define TRA_ENGINE_MESSAGE_SYSTEM_HPP

#include "iNetworkSystem.hpp"

namespace tra::engine
{
	struct SendTcpMessageSystem : public INetworkSystem
	{
		void update(NetworkEcs* _ecs) override;
	};

	struct ReceiveTcpMessageSystem : public INetworkSystem
	{
		void update(NetworkEcs* _ecs) override;
	};
}

#endif
