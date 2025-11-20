#include "disconnectSystem.hpp"

#include "TME/core/tcpSocket.hpp"

#include "TME/engine/networkEcs.hpp"

#include "TME/engine/disconnectedComponent.hpp"
#include "selfComponent.hpp"

namespace tme::engine
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
				TME_ERROR_LOG("DisconnectSystem::update: Failed to remove DisconnectedComponentTag from entity %llu, ErrorCode: %d",
					static_cast<unsigned long long>(entityId), static_cast<int>(removeResult));
			}

			if (!_ecs->hasComponent<SelfComponentTag>(entityId))
			{
				_ecs->destroyEntity(entityId);
			}
		}
	}
}