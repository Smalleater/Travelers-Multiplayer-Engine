#include "ServiceManager.hpp"

#include <memory>

#include "Utils.hpp"

#include "ServiceLocator.hpp"
#include "loggers/CompositeLogger.hpp"
#include "loggers/ConsoleLogger.hpp"
#include "loggers/FileLogger.hpp"
#include "ThreadManager.hpp"

namespace tme
{
    // Initialize all core services
    ErrorCodes ServiceManager::Init()
    {
        // Create a composite logger to combine console and file outputs
        std::unique_ptr<CompositeLogger> compositeLogger = std::make_unique<CompositeLogger>();

        // Optionally add a console logger if the output is supported
        if (Utils::HasAnyConsoleOutput())
        {
            std::unique_ptr<ConsoleLogger> consoleLogger = std::make_unique<ConsoleLogger>();
            compositeLogger->addLogger(std::move(consoleLogger));
        }

        // Always add a file logger
        std::unique_ptr<FileLogger> fileLogger = std::make_unique<FileLogger>();
        compositeLogger->addLogger(std::move(fileLogger));

        // Provide the logger to the service locator
        ServiceLocator::ProvideLogger(std::move(compositeLogger));
        ServiceLocator::Logger().LogInfo("Logger started successfully");

        // Create and register the thread manager
        std::unique_ptr<ThreadManager> threadManager = std::make_unique<ThreadManager>();
        ServiceLocator::ProvideThreadManager(std::move(threadManager));

        // Initialize the thread manager
        ServiceLocator::ThreadManager().Init();

        // Submit a job to confirm thread manager functionality
        ServiceLocator::ThreadManager().SubmitJob([]()
        {
            ServiceLocator::Logger().LogInfo("ThreadManager started successfully");
        });

        ServiceLocator::Logger().LogInfo("All services have ben started successfully");
        return ErrorCodes::Success;
    }

    // Shut down all services cleanly
    ErrorCodes ServiceManager::ShutDown()
    {
        ErrorCodes result = ErrorCodes::Success;

        // Shut down the thread manager
        ServiceLocator::ThreadManager().Shutdown();

        // Log shutdown outcome and update result if there were errors
        if (result == ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogInfo("Shutdown of all services completed successgully");
        }
        else
        {
            result = ErrorCodes::CompletedWithErrors;
            ServiceLocator::Logger().LogWarning("Shutdown of all services completed with erros");
        }

        // Reset the service locator
        ServiceLocator::Reset();

        return result;
    }
}