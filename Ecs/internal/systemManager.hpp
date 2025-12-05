#ifndef TRA_ECS_SYSTEM_MANAGER_HPP
#define TRA_ECS_SYSTEM_MANAGER_HPP

#include <vector>
#include <memory>

#include "TRA/ecs/iSystem.hpp"

namespace tra::ecs
{
	class Engine;

	class SystemManager
	{
	public:
		SystemManager() = default;
		~SystemManager() = default;

		void addBeginUpdateSystem(ISystem* _system);
		void addEndUpdateSystem(ISystem* _system);

		void beginUpdate(Engine* _engine);
		void endUpdate(Engine* _engine);

	private:
		std::vector<std::unique_ptr<ISystem>> m_beginUpdateSystems;
		std::vector<std::unique_ptr<ISystem>> m_endUpdateSystems;
	};
}

#endif
