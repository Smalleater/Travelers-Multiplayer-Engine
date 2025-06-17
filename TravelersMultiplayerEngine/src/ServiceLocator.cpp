#include "TME/ServiceLocator.h"

#include <stdexcept>

namespace tme
{
	std::shared_ptr<IThreadManager> ServiceLocator::m_threadManager = nullptr;

	void ServiceLocator::provideThreadManager(std::shared_ptr<IThreadManager> manager)
	{
		m_threadManager = std::move(manager);
	}

	IThreadManager& ServiceLocator::threadManager()
	{
		if (!m_threadManager)
		{
			throw std::runtime_error("ThreadManager not provided to ServiceLocator.");
		}

		return *m_threadManager;
	}

	void ServiceLocator::reset()
	{
		m_threadManager.reset();
	}
}