#include "acceptConnectionSystem.hpp"

#define MAX_ACCEPTED_CONNECTIONS_PAR_TICK 32

#include "TME/debugUtils.hpp"

#include "TME/core/tcpSocket.hpp"

#include "TME/engine/networkEcs.hpp"
#include "TME/engine/networkEcsUtils.hpp"
#include "TME/engine/networkRootComponentTag.hpp"

#include "socketComponent.hpp"
#include "messageComponent.hpp"

namespace tme::engine
{
	void AcceptConnectionSystem::update(NetworkEcs* _ecs)
	{
		uint8_t acceptedConnections = 0;

		EntityId querryEntityid = 0;
		std::shared_ptr<TcpListenSocketComponent> tcpListenSocketComponent = nullptr;

		core::TcpSocket* clientSocket = nullptr;
		EntityId newEntityId = 0;
		ErrorCode errorCode = ErrorCode::Success;
		std::shared_ptr<NetworkRootComponentTag> networkRootComponentTag = nullptr;
		std::shared_ptr<TcpConnectSocketComponent> tcpSocketComponent = nullptr;
		std::shared_ptr<SendTcpMessageComponent> sendMessageComponent = nullptr;
		std::shared_ptr<ReceiveTcpMessageComponent> receiveMessageComponent = nullptr;

		for (auto queryResult : _ecs->query<TcpListenSocketComponent>())
		{
			querryEntityid = std::get<0>(queryResult);
			tcpListenSocketComponent = std::get<1>(queryResult);

			acceptedConnections = 0;
			while (acceptedConnections < MAX_ACCEPTED_CONNECTIONS_PAR_TICK)
			{
				clientSocket = nullptr;
				std::pair<ErrorCode, int> intPairResult = tcpListenSocketComponent->m_tcpSocket->acceptSocket(&clientSocket);
				if (intPairResult.first == ErrorCode::SocketWouldBlock)
				{
					break;
				}
				else if (intPairResult.first != ErrorCode::Success)
				{
					TME_ERROR_LOG("AcceptConnectionSystem::update: Failed to accept new connection on entity %llu, ErrorCode: %d, Last socket error: %d",
						static_cast<unsigned long long>(querryEntityid), static_cast<int>(intPairResult.first), static_cast<int>(intPairResult.second));
					break;
				}

				newEntityId = _ecs->createEntity();

				TME_ENTITY_ADD_COMPONENT(_ecs, newEntityId, std::make_shared<NetworkRootComponentTag>(), {
					clientSocket->closeSocket();
					delete clientSocket;
					_ecs->destroyEntity(newEntityId);
					continue;
					});

				tcpSocketComponent = std::make_shared<TcpConnectSocketComponent>();
				tcpSocketComponent->m_tcpSocket = clientSocket;
				TME_ENTITY_ADD_COMPONENT(_ecs, newEntityId, tcpSocketComponent, {
					clientSocket->closeSocket();
					delete clientSocket;
					_ecs->destroyEntity(newEntityId);
					continue;
					});

				TME_ENTITY_ADD_COMPONENT(_ecs, newEntityId, std::make_shared<ReceiveTcpMessageComponent>(), {
					clientSocket->closeSocket();
					delete clientSocket;
					_ecs->destroyEntity(newEntityId);
					continue;
					});

				sendMessageComponent = std::make_shared<SendTcpMessageComponent>();
				sendMessageComponent->m_lastMessageByteSent = 0;
				TME_ENTITY_ADD_COMPONENT(_ecs, newEntityId, sendMessageComponent, {
					clientSocket->closeSocket();
					delete clientSocket;
					_ecs->destroyEntity(newEntityId);
					continue;
					});

				tcpSocketComponent.reset();

				acceptedConnections++;
				TME_INFO_LOG("AcceptConnectionSystem::update: Accepted new TCP connection. Entity ID: %llu", newEntityId);
			}
		}
	}
}