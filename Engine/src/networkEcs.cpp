#include "TRA/engine/networkEcs.hpp"

#include "TRA/engine/networkEcsUtils.hpp"

#include "iNetworkSystem.hpp"
#include "destroyComponentTag.hpp"

namespace tra::engine
{
	EntityId NetworkEcs::createEntity()
	{
		EntityId newEntityId = m_nextEntityId++;
		m_entities.insert(newEntityId);

		return newEntityId;
	}

	bool NetworkEcs::isEntityValid(EntityId _entityId)
	{
		return m_entities.find(_entityId) == m_entities.end() && hasComponent<DestroyComponentTag>(_entityId);
	}

	void NetworkEcs::destroyEntity(EntityId _entityId)
	{
		if (m_entities.find(_entityId) == m_entities.end() || hasComponent<DestroyComponentTag>(_entityId))
		{
			return;
		}

		TRA_ENTITY_ADD_COMPONENT(this, _entityId, std::make_shared<DestroyComponentTag>(), {});
	}

	void NetworkEcs::registerBeginUpdateSystem(std::shared_ptr<INetworkSystem> _system)
	{
		if (m_registerBeginUpdateSystem.find(_system) != m_registerBeginUpdateSystem.end())
		{
			TRA_DEBUG_LOG("System already registered in begin update systems.");
			return;
		}

		m_registerBeginUpdateSystem.insert(_system);
		m_beginUpdateSystems.push_back(_system);

		return;
	}

	void NetworkEcs::registerEndUpdateSystem(std::shared_ptr<INetworkSystem> _system)
	{
		if (m_registerEndUpdateSystem.find(_system) != m_registerEndUpdateSystem.end())
		{
			TRA_DEBUG_LOG("System already registered in end update systems.");
			return;
		}

		m_registerEndUpdateSystem.insert(_system);
		m_endUpdateSystems.push_back(_system);

		return;
	}

	void NetworkEcs::beginUpdate()
	{
		for (size_t i = 0; i < m_beginUpdateSystems.size(); i++)
		{
			m_beginUpdateSystems[i]->update(this);
		}
	}

	void NetworkEcs::endUpdate()
	{
		for (size_t i = 0; i < m_endUpdateSystems.size(); i++)
		{
			m_endUpdateSystems[i]->update(this);
		}

		for (auto entityId : queryIds<DestroyComponentTag>())
		{
			for (auto store : m_componentStores)
			{
				std::static_pointer_cast<SparseSet<INetworkComponent>>(store.second)->remove(entityId);
			}

			m_entities.erase(entityId);
		}
	}
}