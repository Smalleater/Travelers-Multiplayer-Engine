#include "pendingDisconnectSystem.hpp"

#include "TME/engine/networkEcs.hpp"
#include "TME/engine/networkEcsUtils.hpp"

#include "TME/engine/connectionStatusComponent.hpp"
#include "TME/engine/disconnectedComponent.hpp"
#include "pendingDisconnectComponent.hpp"
#include "socketComponent.hpp"
#include "messageComponent.hpp"

namespace tme::engine
{
	void PendingDisconnectSystem::update(NetworkEcs* _ecs)
	{
		EntityId entityId = 0;
		ErrorCode removeResult;

		for (auto queryResult : _ecs->query<PendingDisconnectComponentTag, TcpConnectSocketComponent>())
		{
			entityId = std::get<0>(queryResult);

			removeResult = _ecs->removeComponentFromEntity<PendingDisconnectComponentTag>(entityId);
			if (removeResult != ErrorCode::Success)
			{
				TME_ERROR_LOG("DisconnectSystem::update: Failed to remove PendingDisconnectComponentTag from entity %I32u, ErrorCode: %d",
					entityId, static_cast<int>(removeResult));
			}

			removeResult = _ecs->removeComponentFromEntity<TcpConnectSocketComponent>(entityId);
			if (removeResult != ErrorCode::Success)
			{
				TME_ERROR_LOG("DisconnectSystem::update: Failed to remove TcpConnectSocketComponent from entity %I32u, ErrorCode: %d",
					entityId, static_cast<int>(removeResult));
			}

			removeResult = _ecs->removeComponentFromEntity<SendTcpMessageComponent>(entityId);
			if (removeResult != ErrorCode::Success)
			{
				TME_ERROR_LOG("DisconnectSystem::update: Failed to remove SendTcpMessageComponent from entity %I32u, ErrorCode: %d",
					entityId, static_cast<int>(removeResult));
			}

			removeResult = _ecs->removeComponentFromEntity<ReceiveTcpMessageComponent>(entityId);
			if (removeResult != ErrorCode::Success)
			{
				TME_ERROR_LOG("DisconnectSystem::update: Failed to remove ReceiveTcpMessageComponent from entity %I32u, ErrorCode: %d",
					entityId, static_cast<int>(removeResult));
			}

			removeResult = _ecs->removeComponentFromEntity<ConnectedComponentTag>(entityId);
			if (removeResult != ErrorCode::Success)
			{
				TME_ERROR_LOG("DisconnectSystem::update: Failed to remove ConnectedComponentTag from entity %I32u, ErrorCode: %d",
					entityId, static_cast<int>(removeResult));
			}

			TME_ENTITY_ADD_COMPONENT(_ecs, entityId, std::make_shared<DisconnectedComponentTag>(), {});
			TME_INFO_LOG("NetworkEngine: Entity ID: %I32u disconnected", entityId);
		}
	}
}
