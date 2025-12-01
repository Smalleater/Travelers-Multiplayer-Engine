#include "messageSystem.hpp"

#define TME_MAX_TCP_MESSAGES_TO_RECEIVE_PAR_TICK 32

#include "TME/debugUtils.hpp"

#include "TME/core/tcpSocket.hpp"

#include "TME/engine/networkEcs.hpp"
#include "TME/engine/networkEcsUtils.hpp"

#include "messageSerializer.hpp"

#include "socketComponent.hpp"
#include "messageComponent.hpp"
#include "pendingDisconnectComponent.hpp"

namespace tme::engine
{
	void SendTcpMessageSystem::update(NetworkEcs* _ecs)
	{
		EntityId entityId = 0;

		std::shared_ptr<TcpConnectSocketComponent> tcpSocketComponent = nullptr;
		std::shared_ptr<SendTcpMessageComponent> sendTcpMessageComponent = nullptr;

		std::vector<uint8_t> serializedMessage;

		for (auto queryResult : _ecs->query<TcpConnectSocketComponent, SendTcpMessageComponent>())
		{
			entityId = std::get<0>(queryResult);
			if (_ecs->hasComponent<PendingDisconnectComponentTag>(entityId))
			{
				continue;
			}

			tcpSocketComponent = std::get<1>(queryResult);
			sendTcpMessageComponent = std::get<2>(queryResult);

			for (auto message : sendTcpMessageComponent->m_messagesToSend)
			{
				serializedMessage.clear();
				serializedMessage = MessageSerializer::serializePayload(*message.get());
				serializedMessage = MessageSerializer::serializeForNetwork(serializedMessage);
				sendTcpMessageComponent->m_serializedToSend.push_back(serializedMessage);
			}

			sendTcpMessageComponent->m_messagesToSend.clear();

			for (auto messageIt = sendTcpMessageComponent->m_serializedToSend.begin(); messageIt != sendTcpMessageComponent->m_serializedToSend.end();)
			{
				int byteSent = 0;
				auto sendDataResult = tcpSocketComponent->m_tcpSocket->sendData(messageIt->data() + sendTcpMessageComponent->m_lastMessageByteSent,
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
						TME_ENTITY_ADD_COMPONENT(_ecs, entityId, std::make_shared<PendingDisconnectComponentTag>(), {});
					}
					else
					{
						TME_ERROR_LOG("SendTcpMessageSystem::update: Failed to send data for entity %llu, ErrorCode: %d, Last socket error: %d",
							static_cast<unsigned long long>(entityId), static_cast<int>(sendDataResult.first), static_cast<int>(sendDataResult.second));

						TME_ENTITY_ADD_COMPONENT(_ecs, entityId, std::make_shared<PendingDisconnectComponentTag>(), {});
					}

					break;
				}

				messageIt = sendTcpMessageComponent->m_serializedToSend.erase(messageIt);
			}
		}
	}

	void ReceiveTcpMessageSystem::update(NetworkEcs* _ecs)
	{
		uint8_t messagesReceived = 0;

		EntityId entityId = 0;
		std::shared_ptr<TcpConnectSocketComponent> tcpSocketComponent = nullptr;
		std::shared_ptr<ReceiveTcpMessageComponent> receiveTcpMessageComponent = nullptr;

		std::vector<uint8_t> newReceivedBuffer;
		std::vector<uint8_t> payload;

		size_t consumedBytes = 0;

		for (auto queryResult : _ecs->query<TcpConnectSocketComponent, ReceiveTcpMessageComponent>())
		{
			entityId = std::get<0>(queryResult);
			if (_ecs->hasComponent<PendingDisconnectComponentTag>(entityId))
			{
				continue;
			}

			tcpSocketComponent = std::get<1>(queryResult);
			receiveTcpMessageComponent = std::get<2>(queryResult);

			receiveTcpMessageComponent->m_receivedMessages.clear();

			auto receiveDataResult = tcpSocketComponent->m_tcpSocket->receiveData(newReceivedBuffer);
			if (receiveDataResult.first != ErrorCode::Success && receiveDataResult.first != ErrorCode::SocketWouldBlock)
			{
				if (receiveDataResult.first == ErrorCode::SocketConnectionClosed)
				{
					TME_ENTITY_ADD_COMPONENT(_ecs, entityId, std::make_shared<PendingDisconnectComponentTag>(), {});
					continue;
				}
				else
				{
					TME_ERROR_LOG("ReceiveTcpMessageSystem::update: Failed to receive data for entity %llu, ErrorCode: %d, Last socket error: %d",
						static_cast<unsigned long long>(entityId), static_cast<int>(receiveDataResult.first), static_cast<int>(receiveDataResult.second));
					
					TME_ENTITY_ADD_COMPONENT(_ecs, entityId, std::make_shared<PendingDisconnectComponentTag>(), {});
					continue;
				}
			}

			receiveTcpMessageComponent->m_receivedBuffer.insert(receiveTcpMessageComponent->m_receivedBuffer.end(),
				newReceivedBuffer.begin(), newReceivedBuffer.end());

			messagesReceived = 0;
			while (messagesReceived < TME_MAX_TCP_MESSAGES_TO_RECEIVE_PAR_TICK)
			{
				if (!MessageSerializer::getPayloadFromNetworkBuffer(receiveTcpMessageComponent->m_receivedBuffer, payload, consumedBytes))
				{
					break;
				}

				receiveTcpMessageComponent->m_receivedBuffer.erase(receiveTcpMessageComponent->m_receivedBuffer.begin(), 
					receiveTcpMessageComponent->m_receivedBuffer.begin() + static_cast<std::vector<uint8_t>::difference_type>(consumedBytes));

				std::shared_ptr<Message> newMessage = MessageSerializer::deserializePayload(payload);
				if (!newMessage)
				{
					TME_ERROR_LOG("ReceiveTcpMessageSystem::update: Failed to deserialize message for entity %llu",
						static_cast<unsigned long long>(entityId));
					continue;
				}

				receiveTcpMessageComponent->m_receivedMessages[newMessage->getType()].push_back(newMessage);
				++messagesReceived;
			}
		}
	}
}