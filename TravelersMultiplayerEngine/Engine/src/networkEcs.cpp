#include "TME/engine/networkEcs.hpp"

#include "TME/debugUtils.hpp" 

namespace tme::engine
{
	EntityId NetworkEcs::createEntity()
	{
		EntityId newEntityId = m_nextEntityId++;
		m_entitiesToCreate.insert(newEntityId);
		return newEntityId;
	}

	bool NetworkEcs::isEntityValid(EntityId _entityId) const
	{
		return m_entities.find(_entityId) == m_entities.end() || isEntityScheduledForDestruction(_entityId);
	}

	void NetworkEcs::destroyEntity(EntityId _entityId)
	{
		if (m_entities.find(_entityId) == m_entities.end())
		{
			return;
		}

		m_entities.erase(_entityId);
	}

	void NetworkEcs::registerBeginUpdateSystem(std::shared_ptr<INetworkSystem> _system)
	{
		if (m_registerBeginUpdateSystem.find(_system) == m_registerBeginUpdateSystem.end())
		{
			TME_DEBUG_LOG("System already registered in begin update systems.");
			return;
		}

		m_registerBeginUpdateSystem.insert(_system);
		m_beginUpdateSystems.push_back(_system);

		return;
	}

	void NetworkEcs::registerEndUpdateSystem(std::shared_ptr<INetworkSystem> _system)
	{
		if (m_registerEndUpdateSystem.find(_system) == m_registerEndUpdateSystem.end())
		{
			TME_DEBUG_LOG("System already registered in end update systems.");
			return;
		}

		m_registerEndUpdateSystem.insert(_system);
		m_endUpdateSystems.push_back(_system);

		return;
	}

	bool NetworkEcs::isEntityScheduledForCreation(EntityId _entityId) const
	{
		return m_entitiesToCreate.find(_entityId) != m_entitiesToCreate.end();
	}

	bool NetworkEcs::isEntityScheduledForDestruction(EntityId _entityId) const
	{
		return m_entitiesToDestroy.find(_entityId) != m_entitiesToDestroy.end();
	}
}