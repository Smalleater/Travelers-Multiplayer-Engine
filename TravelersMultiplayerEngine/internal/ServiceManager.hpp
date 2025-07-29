#ifndef TME_SERVICE_MANAGER_HPP
#define TME_SERVICE_MANAGER_HPP

#include "TME/ErrorCodes.hpp"

namespace tme
{
    /// @brief Utility class responsible for initializing and shutting down core engine services.
    ///
    /// Handles startup and cleanup of essential subsystems such as logging and threading.
    class ServiceManager
    {
    public:
        /// @brief Initializes all core services used by the engine.
        ///
        /// This includes setting up the logger system and thread manager.
        ///
        /// @return ErrorCodes Returns ErrorCodes::Success if all services were initialized successfully.
        /// Returns a specific error code if any service failed to initialize.
        static ErrorCodes Init();

        /// @brief Shuts down all registered services gracefully.
        ///
        /// This stops the thread manager, resets the service locator, and logs the shutdown status.
        ///
        /// @return ErrorCodes Returns ErrorCodes::Success if all services shut down cleanly,
        /// or ErrorCodes::CompletedWithErrors if some services did not shut down completely.
        static ErrorCodes ShutDown();
    };
}

#endif