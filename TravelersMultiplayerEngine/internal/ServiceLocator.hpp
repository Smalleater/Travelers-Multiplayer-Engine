#ifndef TME_SERVICE_LOCATOR_HPP
#define TME_SERVICE_LOCATOR_HPP

#include <memory>

#include "interfaces/IThreadManager.hpp"
#include "interfaces/ILogger.hpp"

namespace tme
{
    /// @brief Static service locator to access global engine services.
    ///
    /// Provides a global access point to key engine components while keeping dependencies decoupled.
    class ServiceLocator
    {
    private:
        /// @brief Holds the current ThreadManager instance.
        static std::unique_ptr<IThreadManager> m_threadManager;

        /// @brief Holds the current Logger instance.
        static std::unique_ptr<ILogger> m_logger;

    public:
        /// @brief Registers a ThreadManager implementation with the locator.
        /// @param manager A unique pointer to the ThreadManager instance.
        static void ProvideThreadManager(std::unique_ptr<IThreadManager> manager);

        /// @brief Retrives the currently registered ThreadManager.
        /// @return A reference to the registered ThreadManager.
        /// @throws std::runtime_error if no instance has been provided.
        static IThreadManager& ThreadManager();

        /// @brief Registers a Logger implementation with the locator.
        /// @param logger A unique pointer to the Logger instance.
        static void ProvideLogger(std::unique_ptr<ILogger> logger);

        /// @brief Retrieves the currently registered Logger.
        /// @return A reference to the registered Logger.
        /// @throws std::runtime_error if no instance has been provided.
        static ILogger& Logger();

        /// @brief Clears all registered services.
        ///
        /// Used typically durring shutdown or reinitialization.
        static void Reset();
    };
}

#endif