#include "TME/Engine.h"

#include "TME/ServiceLocator.h"
#include "TME/Logger.h"
#include "TME/ConsoleLogger.h"

namespace tme
{
	bool Engine::start()
	{
		std::shared_ptr<Logger> logger = std::make_shared<Logger>();
		ServiceLocator::provideLogger(logger);

		std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
		logger->addLogger(consoleLogger);

		return true;
	}

	bool Engine::stop()
	{
		return true;
	}
}
