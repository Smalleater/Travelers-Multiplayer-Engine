#include "messageSystem.hpp"

#include "TME/debugUtils.hpp"

#include "TME/engine/networkEcs.hpp"
#include "TME/engine/message.hpp"

#include "messageSerializer.hpp"

#include "tcpSocketComponent.hpp"
#include "messageComponent.hpp"

namespace tme::engine
{
	void SendTcpMessageSystem::update(NetworkEcs& _ecs)
	{
		std::vector<EntityId> entityIds = _ecs.queryEntitiesWithComponent<SendTcpMessageComponent>();

		// Prepared for multi threading in the future
		for (EntityId entityId : entityIds)
		{
			auto getSendTcpMessageComponentResult = _ecs.getComponentOfEntity<SendTcpMessageComponent>(entityId);
			if (getSendTcpMessageComponentResult.first != ErrorCode::Success)
			{
				TME_ERROR_LOG("SendTcpMessageSystem::update: Failed to get SendTcpMessageComponent for entity %llu", static_cast<unsigned long long>(entityId));
				continue;
			}

			auto sendTcpMessageComponent = getSendTcpMessageComponentResult.second.lock();
			if (!sendTcpMessageComponent)
			{
				TME_ERROR_LOG("SendTcpMessageSystem::update: SendTcpMessageComponent is expired for entity %llu", static_cast<unsigned long long>(entityId));
				continue;
			}

			std::vector<uint8_t> serializedMessage;
			for (auto messages : sendTcpMessageComponent->m_messagesToSend)
			{
				for (auto message : messages.second)
				{
					serializedMessage.clear();
					serializedMessage = MessageSerializer::serializePayload(*message.get());
					serializedMessage = MessageSerializer::serializeForNetwork(serializedMessage);
					sendTcpMessageComponent->m_serializedToSend[messages.first].push_back(serializedMessage);
				}
			}
		}

		for (EntityId entityId : entityIds)
		{
			auto getSendTcpMessageComponentResult = _ecs.getComponentOfEntity<SendTcpMessageComponent>(entityId);
			if (getSendTcpMessageComponentResult.first != ErrorCode::Success)
			{
				TME_ERROR_LOG("SendTcpMessageSystem::update: Failed to get SendTcpMessageComponent for entity %llu", static_cast<unsigned long long>(entityId));
				continue;
			}

			auto sendTcpMessageComponent = getSendTcpMessageComponentResult.second.lock();
			if (!sendTcpMessageComponent)
			{
				TME_ERROR_LOG("SendTcpMessageSystem::update: SendTcpMessageComponent is expired for entity %llu", static_cast<unsigned long long>(entityId));
				continue;
			}

			for (auto messages : sendTcpMessageComponent->m_serializedToSend)
			{
				auto getTcpSocketComponent = _ecs.getComponentOfEntity<TcpSocketComponent>(messages.first);
				if (getTcpSocketComponent.first != ErrorCode::Success)
				{

				}
			}
		}
	}
}