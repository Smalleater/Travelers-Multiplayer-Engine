#include "TME/Engine.h"

#include "TME/ServiceLocator.h"
#include "TME/Logger.h"
#include "TME/ConsoleLogger.h"
#include "TME/FileLogger.h"
#include "TME/ThreadManager.h"
#include "TME/Server.h"

namespace tme
{
	bool Engine::m_isRunning = false;

	bool Engine::start()
	{
		std::shared_ptr<Logger> logger = std::make_shared<Logger>();
		ServiceLocator::provideLogger(logger);

		std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
		logger->addLogger(consoleLogger);

		std::shared_ptr<FileLogger> fileLogger = std::make_shared<FileLogger>();
		logger->addLogger(fileLogger);

		ServiceLocator::logger().logInfo("Logger started successfully");

		std::shared_ptr<tme::ThreadManager> threadManager = std::make_shared<tme::ThreadManager>();
		threadManager->start();
		tme::ServiceLocator::provideThreadManager(threadManager);

		ServiceLocator::threadManager().addJob([]() 
			{
				ServiceLocator::logger().logInfo("ThreadManager started successfully");
			});

		m_isRunning = true;

		ServiceLocator::logger().logInfo("The engine has been started successfully");
		return true;
	}

	bool Engine::stop()
	{
		m_isRunning = false;

		if (Server::isRunning())
		{
			Server::stop();
		}

		tme::ServiceLocator::threadManager().stop();
		tme::ServiceLocator::reset();

		return true;
	}
}
