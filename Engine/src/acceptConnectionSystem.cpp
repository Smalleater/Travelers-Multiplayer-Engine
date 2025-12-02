#include "acceptConnectionSystem.hpp"

#define MAX_ACCEPTED_CONNECTIONS_PAR_TICK 32

#include "TRA/debugUtils.hpp"

#include "TRA/core/tcpSocket.hpp"

#include "TRA/engine/networkEcs.hpp"
#include "TRA/engine/networkEcsUtils.hpp"

#include "TRA/engine/networkRootComponentTag.hpp"
#include "TRA/engine/newConnectionComponent.hpp"
#include "TRA/engine/connectionStatusComponent.hpp"

#include "socketComponent.hpp"
#include "messageComponent.hpp"

namespace tra::engine
{
	void AcceptConnectionSystem::update(NetworkEcs* _ecs)
	{
		uint8_t acceptedConnections = 0;

		EntityId querryEntityid = 0;
		std::shared_ptr<NewConnectionComponentTag> newConnectionComponent = nullptr;

		for (auto queryResult : _ecs->query<NewConnectionComponentTag>())
		{
			querryEntityid = std::get<0>(queryResult);
			newConnectionComponent = std::get<1>(queryResult);

			ErrorCode removeResult = _ecs->removeComponentFromEntity<NewConnectionComponentTag>(querryEntityid);
			if (removeResult != ErrorCode::Success)
			{
				TRA_ERROR_LOG("AcceptConnectionSystem::update: Failed to remove NewConnectionComponentTag from entity %I32u. ErrorCode: %d",
					querryEntityid, static_cast<int>(removeResult));
			}
		}

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
					TRA_ERROR_LOG("AcceptConnectionSystem::update: Failed to accept new connection on entity %I32u, ErrorCode: %d, Last socket error: %d",
						querryEntityid, static_cast<int>(intPairResult.first), static_cast<int>(intPairResult.second));
					break;
				}

				auto setblockingResult = clientSocket->setBlocking(false);
				if (setblockingResult.first != ErrorCode::Success)
				{
					TRA_ERROR_LOG("AcceptConnectionSystem::update: Failed to set non-blocking mode on accepted socket. ErrorCode: %d, Last socket error: %d",
						static_cast<int>(setblockingResult.first), static_cast<int>(setblockingResult.second));
					clientSocket->closeSocket();
					delete clientSocket;
					continue;
				}

				newEntityId = _ecs->createEntity();

				TRA_ENTITY_ADD_COMPONENT(_ecs, newEntityId, std::make_shared<NetworkRootComponentTag>(), {
					clientSocket->closeSocket();
					delete clientSocket;
					_ecs->destroyEntity(newEntityId);
					continue;
					});

				tcpSocketComponent = std::make_shared<TcpConnectSocketComponent>();
				tcpSocketComponent->m_tcpSocket = clientSocket;
				TRA_ENTITY_ADD_COMPONENT(_ecs, newEntityId, tcpSocketComponent, {
					clientSocket->closeSocket();
					delete clientSocket;
					_ecs->destroyEntity(newEntityId);
					continue;
					});

				TRA_ENTITY_ADD_COMPONENT(_ecs, newEntityId, std::make_shared<ReceiveTcpMessageComponent>(), {
					clientSocket->closeSocket();
					delete clientSocket;
					_ecs->destroyEntity(newEntityId);
					continue;
					});

				sendMessageComponent = std::make_shared<SendTcpMessageComponent>();
				sendMessageComponent->m_lastMessageByteSent = 0;
				TRA_ENTITY_ADD_COMPONENT(_ecs, newEntityId, sendMessageComponent, {
					clientSocket->closeSocket();
					delete clientSocket;
					_ecs->destroyEntity(newEntityId);
					continue;
					});

				TRA_ENTITY_ADD_COMPONENT(_ecs, newEntityId, std::make_shared<NewConnectionComponentTag>(), {
					clientSocket->closeSocket();
					delete clientSocket;
					_ecs->destroyEntity(newEntityId);
					continue;
					});

				TRA_ENTITY_ADD_COMPONENT(_ecs, newEntityId, std::make_shared<ConnectedComponentTag>(), {
					clientSocket->closeSocket();
					delete clientSocket;
					_ecs->destroyEntity(newEntityId);
					continue;
					});

				tcpSocketComponent.reset();

				acceptedConnections++;
				TRA_INFO_LOG("NetworkEngine: Accepted new TCP connection. Entity ID: %I32u", newEntityId);
			}
		}
	}
}