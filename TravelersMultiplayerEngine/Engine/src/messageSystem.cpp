#include "messageSystem.hpp"

#include "TME/debugUtils.hpp"

#include "TME/core/tcpSocket.hpp"

#include "TME/engine/networkEcs.hpp"

#include "messageSerializer.hpp"

#include "socketComponent.hpp"
#include "messageComponent.hpp"

namespace tme::engine
{
	void SendTcpMessageSystem::update(NetworkEcs* _ecs)
	{
		for (auto queryResult : _ecs->query<TcpConnectSocketComponent, SendTcpMessageComponent>())
		{
			EntityId entityId = std::get<0>(queryResult);
			std::shared_ptr tcpSocketComponent = std::get<1>(queryResult);
			std::shared_ptr sendTcpMessageComponent = std::get<2>(queryResult);

			std::vector<uint8_t> serializedMessage;
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
						tcpSocketComponent->m_tcpSocket->closeSocket();
						_ecs->destroyEntity(entityId);
						TME_INFO_LOG("Engine: Connection closed for entity %llu", static_cast<unsigned long long>(entityId));
					}
					else
					{
						TME_ERROR_LOG("SendTcpMessageSystem::update: Failed to send data for entity %llu, ErrorCode: %d, Last socket error: %d",
							static_cast<unsigned long long>(entityId), static_cast<int>(sendDataResult.first), static_cast<int>(sendDataResult.second));

						tcpSocketComponent->m_tcpSocket->shutdownSocket();
						tcpSocketComponent->m_tcpSocket->closeSocket();
						_ecs->destroyEntity(entityId);
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