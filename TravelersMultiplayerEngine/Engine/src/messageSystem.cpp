#include "messageSystem.hpp"

#include "TME/debugUtils.hpp"

#include "TME/core/tcpSocket.hpp"

#include "TME/engine/networkEcs.hpp"
#include "TME/engine/message.hpp"

#include "messageSerializer.hpp"

#include "tcpSocketComponent.hpp"
#include "messageComponent.hpp"

namespace tme::engine
{
	void SendTcpMessageSystem::update(NetworkEcs& _ecs)
	{
		std::vector<EntityId> entityIds = _ecs.queryEntitiesWithComponent<TcpSocketComponent, SendTcpMessageComponent>();

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
			for (auto message : sendTcpMessageComponent->m_messagesToSend)
			{
				serializedMessage.clear();
				serializedMessage = MessageSerializer::serializePayload(*message.get());
				serializedMessage = MessageSerializer::serializeForNetwork(serializedMessage);
				sendTcpMessageComponent->m_serializedToSend.push_back(serializedMessage);
			}

			sendTcpMessageComponent->m_messagesToSend.clear();

			auto getTcpSocketComponent = _ecs.getComponentOfEntity<TcpSocketComponent>(entityId);
			if (getTcpSocketComponent.first != ErrorCode::Success)
			{
				TME_ERROR_LOG("SendTcpMessageSystem::update: Failed to get TcpSocketComponent for entity %llu", static_cast<unsigned long long>(entityId));
				continue;
			}

			auto tcpSocketComponent = getTcpSocketComponent.second.lock();
			if (!tcpSocketComponent)
			{
				TME_ERROR_LOG("SendTcpMessageSystem::update: TcpSocketComponent is expired for entity %llu", static_cast<unsigned long long>(entityId));
				continue;
			}

			for (auto messageIt = sendTcpMessageComponent->m_serializedToSend.begin(); messageIt != sendTcpMessageComponent->m_serializedToSend.end();)
			{
				int byteSent = 0;
				auto sendDataResult = tcpSocketComponent->tcpSocket->sendData(messageIt->data() + sendTcpMessageComponent->m_lastMessageByteSent,
					messageIt->size() - sendTcpMessageComponent->m_lastMessageByteSent, byteSent);

				if (sendDataResult.first != ErrorCode::Success)
				{
					if (sendDataResult.first == ErrorCode::SocketSendPartial)
					{
						sendTcpMessageComponent->m_lastMessageByteSent = byteSent;
						TME_DEBUG_LOG("SendTcpMessageSystem::update: Partial data sent for entity %llu, BytesSent: %d/%llu",
							static_cast<unsigned long long>(entityId), byteSent, static_cast<unsigned long long>(messageIt->size()));
					}
					else if (sendDataResult.first == ErrorCode::SocketConnectionClosed)
					{
						tcpSocketComponent->tcpSocket->closeSocket();
						_ecs.destroyEntity(entityId);
						TME_INFO_LOG("Engine: Connection closed for entity %llu", static_cast<unsigned long long>(entityId));
					}
					else
					{
						TME_ERROR_LOG("SendTcpMessageSystem::update: Failed to send data for entity %llu, ErrorCode: %d, Last socket error: %d",
							static_cast<unsigned long long>(entityId), static_cast<int>(sendDataResult.first), static_cast<int>(sendDataResult.second));

						tcpSocketComponent->tcpSocket->shutdownSocket();
						tcpSocketComponent->tcpSocket->closeSocket();
						_ecs.destroyEntity(entityId);
						TME_INFO_LOG("Engine: Connection closed for entity %llu", static_cast<unsigned long long>(entityId));
					}

					break;
				}

				TME_DEBUG_LOG("SendTcpMessageSystem::update: Sent data for entity %llu, BytesSent: %d/%llu",
					static_cast<unsigned long long>(entityId), byteSent, static_cast<unsigned long long>(messageIt->size()));

				messageIt = sendTcpMessageComponent->m_serializedToSend.erase(messageIt);
			}
		}
	}
}