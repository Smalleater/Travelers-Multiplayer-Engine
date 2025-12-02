#include "disconnectSystem.hpp"

#include "TRA/core/tcpSocket.hpp"

#include "TRA/engine/networkEcs.hpp"

#include "TRA/engine/disconnectedComponent.hpp"
#include "selfComponent.hpp"

namespace tra::engine
{
	void DisconnectSystem::update(NetworkEcs* _ecs)
	{
		EntityId entityId = 0;
		ErrorCode removeResult;

		for (auto queryResult : _ecs->query<DisconnectedComponentTag>())
		{
			entityId = std::get<0>(queryResult);

			removeResult = _ecs->removeComponentFromEntity<DisconnectedComponentTag>(entityId);
			if (removeResult != ErrorCode::Success)
			{
				TRA_ERROR_LOG("DisconnectSystem::update: Failed to remove DisconnectedComponentTag from entity %llu, ErrorCode: %d",
					static_cast<unsigned long long>(entityId), static_cast<int>(removeResult));
			}

			if (!_ecs->hasComponent<SelfComponentTag>(entityId))
			{
				_ecs->destroyEntity(entityId);
			}
		}
	}
}