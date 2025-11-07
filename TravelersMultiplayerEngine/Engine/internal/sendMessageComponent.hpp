#ifndef TME_ENGINE_SEND_MESSAGE_COMPONENT_HPP
#define TME_ENGINE_SEND_MESSAGE_COMPONENT_HPP

#include <vector>
#include <cstdint>
#include <unordered_map>

#include "TME/engine/iNetworkComponent.hpp"

namespace tme::engine
{
	using EntityId = uint64_t;
	struct Message;

	struct SendTcpMessageComponent : public INetworkComponent
	{
		std::unordered_map<EntityId, std::vector<Message*>> m_messagesToSend;
	};
}

#endif
