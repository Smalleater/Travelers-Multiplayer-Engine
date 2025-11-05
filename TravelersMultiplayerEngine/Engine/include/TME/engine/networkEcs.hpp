#ifndef TME_ENGINE_NETWORK_ECS_HPP
#define TME_ENGINE_NETWORK_ECS_HPP

#include "TME/export.hpp"

#include <vector>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <memory>
#include <utility>

#include "TME/errorCode.hpp"
#include "TME/engine/INetworkComponent.hpp"

namespace tme::engine
{
	using EntityId = uint64_t;

	template<typename ComponentType>
	struct SparseSet
	{
		std::vector<std::shared_ptr<ComponentType>> dense;
		std::unordered_map<EntityId, size_t> sparse;

		void insert(EntityId _entityId, std::shared_ptr<ComponentType> _component)
		{
			sparse[_entityId] = dense.size();
			dense.push_back(_component);
		}

		void remove(EntityId _entityId)
		{
			auto it = sparse.find(_entityId);
			if (it != sparse.end())
			{
				size_t index = it->second;
				dense[index] = dense.back();
				dense.pop_back();
				sparse.erase(it);
			}
		}

		std::pair<ErrorCode, std::weak_ptr<ComponentType>> get(EntityId _entityId)
		{
			auto it = sparse.find(_entityId);
			if (it != sparse.end())
			{
				return { ErrorCode::Success, dense[it->second] };
			}

			return { ErrorCode::EntityDoesNotHaveComponent , std::weak_ptr<ComponentType>() };
		}

		bool hasComponent(EntityId _entityId)
		{
			return sparse.find(_entityId) != sparse.end();
		}
	};

	class NetworkEcs;
	struct INetworkSystem
	{
		virtual ~INetworkSystem() = default;
		virtual void update(NetworkEcs& _ecs) = 0;
	};

	class NetworkEcs
	{
	public:
		NetworkEcs() = default;
		~NetworkEcs() = default;

		EntityId createEntity();
		bool isEntityValid(EntityId _entityId) const;
		void destroyEntity(EntityId _entityId);

		void registerBeginUpdateSystem(std::shared_ptr<INetworkSystem> _system);
		void registerEndUpdateSystem(std::shared_ptr<INetworkSystem> _system);

		template<typename ComponentType>
		ErrorCode addComponentToEntity(EntityId _entityId, std::shared_ptr<ComponentType> _component)
		{
			auto store = getOrCreateComponentStore<ComponentType>().lock();
			if (!store)
			{
				return ErrorCode::ComponentStoreUnavailable;
			}

			if (store->hasComponent(_entityId))
			{
				return ErrorCode::EntityAlreadyHasComponent;
			}

			store->insert(_entityId, _component);

			return ErrorCode::Success;
		}

		template<typename ComponentType>
		bool hasComponent(EntityId _entityId) const
		{
			std::weak_ptr<SparseSet<ComponentType>> store = getOrCreateComponentStore<ComponentType>();
			return store->hasComponent(_entityId);
		}

		template<typename ComponentType>
		std::pair<ErrorCode, std::weak_ptr<ComponentType>> getComponentOfEntity(EntityId _entityId)
		{
			if (isEntityScheduledForCreation(_entityId))
			{
				return { ErrorCode::EntityScheduledForCreation, std::weak_ptr<ComponentType>() };
			}

			std::weak_ptr<SparseSet<ComponentType>> store = getOrCreateComponentStore<ComponentType>();
			return store->get(_entityId);
		}

		template<typename ComponentType>
		std::vector<EntityId> queryEntitiesWithComponent() const
		{
			std::weak_ptr<SparseSet<ComponentType>> store = getOrCreateComponentStore<ComponentType>();
			std::vector<EntityId> entityIds;

			for (const auto& [entityId, index] : store->sparse)
			{
				if (!isEntityScheduledForCreation(_entityId))
				{
					entityIds.push_back(entityId);
				}
			}

			return entityIds;
		}

		template<typename ComponentType>
		ErrorCode removeComponentFromEntity(EntityId _entityId)
		{
			if (isEntityScheduledForCreation(_entityId))
			{
				return ErrorCode::EntityScheduledForCreation;
			}

			std::weak_ptr<SparseSet<ComponentType>> store = getOrCreateComponentStore<ComponentType>();
			store->remove(_entityId);

			return ErrorCode::Success;
		}

	private:
		EntityId m_nextEntityId = 0;
		std::unordered_set<EntityId> m_entities;
		std::unordered_set<EntityId> m_entitiesToCreate;
		std::unordered_set<EntityId> m_entitiesToDestroy;

		std::unordered_map<size_t, std::shared_ptr<void>> m_componentStores;

		std::unordered_set<std::shared_ptr<INetworkSystem>> m_registerBeginUpdateSystem;
		std::unordered_set<std::shared_ptr<INetworkSystem>> m_registerEndUpdateSystem;
		std::vector<std::shared_ptr<INetworkSystem>> m_beginUpdateSystems;
		std::vector<std::shared_ptr<INetworkSystem>> m_endUpdateSystems;

		bool isEntityScheduledForCreation(EntityId _entityId) const;
		bool isEntityScheduledForDestruction(EntityId _entityId) const;

		template<typename ComponentType>
		std::weak_ptr<SparseSet<ComponentType>> getOrCreateComponentStore()
		{
			static_assert(std::is_base_of<INetworkComponent, ComponentType>::value, "ComponentType must derive from INetworkComponent");

			size_t hashCode = typeid(ComponentType).hash_code();
			auto it = m_componentStores.find(hashCode);
			if (it == m_componentStores.end())
			{
				std::shared_ptr<SparseSet<ComponentType>> newStore = std::make_shared<SparseSet<ComponentType>>();
				m_componentStores[hashCode] = newStore;
				return std::weak_ptr<SparseSet<ComponentType>>(newStore);
			}

			return std::static_pointer_cast<SparseSet<ComponentType>>(it->second);
		}
	};
}

#endif
