#ifndef TME_ENGINE_MESSAGE_COMPONENT_HPP
#define TME_ENGINE_MESSAGE_COMPONENT_HPP

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
		std::vector<std::shared_ptr<Message>> m_messagesToSend;
		std::vector<std::vector<uint8_t>> m_serializedToSend;
		int m_lastMessageByteSent;
	};

	struct ReceiveTcpMessageComponent : public INetworkComponent
	{
		std::unordered_map<std::string, std::vector<std::shared_ptr<Message>>> m_receivedMessages;
	};
}

#endif
