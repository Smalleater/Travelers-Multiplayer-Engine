#include "ServiceLocator.hpp"

#include <stdexcept>

namespace tme
{
    // Static members used to store unique service instances
    std::unique_ptr<IThreadManager> ServiceLocator::m_threadManager = nullptr;
    std::unique_ptr<ILogger> ServiceLocator::m_logger = nullptr;

    // Registers a new IThreadManager implementation
    void ServiceLocator::ProvideThreadManager(std::unique_ptr<IThreadManager> manager)
    {
        // Transfers ownership of the manager to the Service Locator
        m_threadManager = std::move(manager);
    }

    // Returns a reference to the registered IThreadManager
    IThreadManager& ServiceLocator::ThreadManager()
    {
        // If no ThreadManager has been registered, throw an exception
        if (!m_threadManager)
        {
            throw std::runtime_error("ThreadManager not provided to ServiceLocator");
        }

        // Return a reference to the registered ThreadManager
        return *m_threadManager;
    }

    // Registers a new ILogger implementation
    void ServiceLocator::ProvideLogger(std::unique_ptr<ILogger> logger)
    {
        // Transfers ownership of the logger to the Service Locator
        m_logger = std::move(logger);
    }

    // Returns a reference to the registered ILogger
    ILogger& ServiceLocator::Logger()
    {
        // If no Logger has been registered, throw an exception
        if (!m_logger)
        {
            throw std::runtime_error("Logger not provided to ServiceLocator");
        }

        // Return a reference to the registered Logger
        return *m_logger;
    }

    // Resets all registered services
    void ServiceLocator::Reset()
    {
        m_threadManager.reset();
        m_logger.reset();
    }
}
