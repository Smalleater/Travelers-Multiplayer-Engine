#include "TME/ServiceLocator.h"

#include <stdexcept>

namespace tme
{
	std::shared_ptr<IThreadManager> ServiceLocator::m_threadManager = nullptr;
	std::shared_ptr<ILogger> ServiceLocator::m_logger = nullptr;

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

	void ServiceLocator::provideLogger(std::shared_ptr<ILogger> logger)
	{
		m_logger = std::move(logger);
	}

	ILogger& ServiceLocator::logger()
	{
		if (!m_logger)
		{
			throw std::runtime_error("Logger not provided to ServiceLocator.");
		}

		return *m_logger;
	}

	void ServiceLocator::reset()
	{
		m_threadManager.reset();
		m_logger.reset();
	}
}