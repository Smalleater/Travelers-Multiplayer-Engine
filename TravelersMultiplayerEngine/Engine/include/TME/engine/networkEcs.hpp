#ifndef TME_ENGINE_NETWORK_ECS_HPP
#define TME_ENGINE_NETWORK_ECS_HPP

#include "TME/export.hpp"

#include <vector>
#include <cstdint>

namespace tme::engine
{
	struct NetworkIComponent
	{
		virtual ~NetworkIComponent() = default;
	};

	struct ISerializableComponent : public NetworkIComponent
	{
		virtual std::vector<uint8_t> serialize() const = 0;
		virtual void deserialize(std::vector<uint8_t> _buffer) = 0;
	};

	class NetworkEntity
	{
	public:
		NetworkEntity();
		~NetworkEntity();

		template<typename T, typename... Args>
		T* addComponent(Args&&... args)
		{
			static_assert(std::is_base_of<NetworkIComponent, T>::value, "T must be derived from NetworkIComponent");
			T* component = new T(std::forward<Args>(args)...);
			m_components.push_back(component);
			return component;
		}

		template<typename T>
		T* getComponent()
		{
			static_assert(std::is_base_of<NetworkIComponent, T>::value, "T must be derived from NetworkIComponent");
			for (auto* component : m_components)
			{
				if (T* casted = dynamic_cast<T*>(component))
				{
					return casted;
				}
			}
			return nullptr;
		}

	private:

	};

	class NetworkEcs
	{
	public:
		NetworkEcs();
		~NetworkEcs();

	private:

	};
}

#endif
