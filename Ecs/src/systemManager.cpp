#include "systemManager.hpp"

#include <algorithm>

#include "TRA/debugUtils.hpp"

namespace tra::ecs
{
	void SystemManager::addBeginUpdateSystem(ISystem* _system)
	{
		if (std::find(m_beginUpdateSystems.begin(), m_beginUpdateSystems.end(), _system) == m_beginUpdateSystems.end())
		{
			TRA_WARNING_LOG("Ecs: Attempted to add duplicate begin update system.");
			return;
		}

		m_beginUpdateSystems.emplace_back(std::unique_ptr<ISystem>(_system));
	}

	void SystemManager::addEndUpdateSystem(ISystem* _system)
	{
		if (std::find(m_endUpdateSystems.begin(), m_endUpdateSystems.end(), _system) == m_endUpdateSystems.end())
		{
			TRA_WARNING_LOG("Ecs: Attempted to add duplicate end update system.");
			return;
		}

		m_beginUpdateSystems.emplace_back(std::unique_ptr<ISystem>(_system));
	}

	void SystemManager::beginUpdate(Engine* _engine)
	{
		for (size_t i = 0; i < m_beginUpdateSystems.size(); i++)
		{
			m_beginUpdateSystems[i]->update(_engine);
		}
	}

	void SystemManager::endUpdate(Engine* _engine)
	{
		for (size_t i = 0; i < m_endUpdateSystems.size(); i++)
		{
			m_endUpdateSystems[i]->update(_engine);
		}
	}
}