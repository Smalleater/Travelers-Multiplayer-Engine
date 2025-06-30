#include "TME/Engine.h"

#include "TME/ServiceLocator.h"
#include "TME/Logger.h"
#include "TME/ConsoleLogger.h"
#include "TME/FileLogger.h"
#include "TME/ThreadManager.h"

namespace tme
{
	bool Engine::start()
	{
		std::shared_ptr<Logger> logger = std::make_shared<Logger>();
		ServiceLocator::provideLogger(logger);

		std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
		logger->addLogger(consoleLogger);

		std::shared_ptr<FileLogger> fileLogger = std::make_shared<FileLogger>();
		logger->addLogger(fileLogger);

		std::shared_ptr<tme::ThreadManager> threadManager = std::make_shared<tme::ThreadManager>();
		threadManager->start();
		tme::ServiceLocator::provideThreadManager(threadManager);

		return true;
	}

	bool Engine::stop()
	{
		tme::ServiceLocator::threadManager().stop();
		tme::ServiceLocator::reset();

		return true;
	}
}
