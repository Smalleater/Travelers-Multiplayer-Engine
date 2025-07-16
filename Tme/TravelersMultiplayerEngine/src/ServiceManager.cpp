#include "ServiceManager.h"

#include <memory>

#include "TME/Utils.h"

#include "ServiceLocator.h"
#include "Loggers/CompositeLogger.h"
#include "Loggers/ConsoleLogger.h"
#include "Loggers/FileLogger.h"
#include "ThreadManager.h"

namespace tme
{
	ErrorCodes ServiceManager::Init()
	{
		std::shared_ptr<CompositeLogger> compositeLogger = std::make_shared<CompositeLogger>();
		ServiceLocator::provideLogger(compositeLogger);

		if (Utils::HasAnyConsoleOutput())
		{
			std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
			compositeLogger->addLogger(consoleLogger);
		}

		std::shared_ptr<FileLogger> fileLogger = std::make_shared<FileLogger>();
		compositeLogger->addLogger(fileLogger);

		ServiceLocator::logger().Log("Logger started successfully");

		std::shared_ptr<ThreadManager> threadManager = std::make_shared<ThreadManager>();
		threadManager->Init();
		ServiceLocator::provideThreadManager(threadManager);

		ServiceLocator::threadManager().SubmitJob([]()
			{
				ServiceLocator::logger().Log("ThreadManager started successfully");
			});

		ServiceLocator::logger().Log("All services have been started successfully");
		return ErrorCodes::Success;
	}

	ErrorCodes ServiceManager::ShutDown()
	{
		ErrorCodes result = ErrorCodes::Success;

		ServiceLocator::threadManager().Shutdown();

		if (result == ErrorCodes::Success)
		{
			ServiceLocator::logger().Log("Shutdown of all services completed successfully");
		}
		else
		{
			ServiceLocator::logger().LogError("Shutdown of services completed with errors");
		}

		ServiceLocator::reset();

		return result;
	}
}
