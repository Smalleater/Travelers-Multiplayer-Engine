#include "TME/NetworkEngine.h"

#include <Windows.h>

#include "ServiceLocator.h"
#include "Logger.h"
#include "Loggers/ConsoleLogger.h"
#include "Loggers/FileLogger.h"
#include "ThreadManager.h"

namespace tme
{
	bool NetworkEngine::m_initialized = false;

	NetworkError NetworkEngine::Init()
	{
		std::shared_ptr<Logger> logger = std::make_shared<Logger>();
		ServiceLocator::provideLogger(logger);

		if (HasAnyConsoleOutput())
		{
			std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
			logger->addLogger(consoleLogger);
		}

		std::shared_ptr<FileLogger> fileLogger = std::make_shared<FileLogger>();
		logger->addLogger(fileLogger);

		ServiceLocator::logger().Log("Logger started successfully");

		std::shared_ptr<ThreadManager> threadManager = std::make_shared<ThreadManager>();
		threadManager->Init();
		ServiceLocator::provideThreadManager(threadManager);

		ServiceLocator::threadManager().SubmitJob([]()
			{
				ServiceLocator::logger().Log("ThreadManager started successfully");
			});

		m_initialized = true;

		ServiceLocator::logger().Log("Network engine has been started successfully");
		return NetworkError::Succes;
	}

	NetworkError NetworkEngine::Shutdown()
	{
		m_initialized = false;

		ServiceLocator::threadManager().Shutdown();
		ServiceLocator::reset();

		return NetworkError::Succes;
	}

	NetworkError NetworkEngine::EnsureInitialized()
	{
		if (m_initialized)
		{
			return NetworkError::Succes;
		}

		return Init();
	}

	bool NetworkEngine::IsInitialized()
	{
		return m_initialized;
	}

	bool NetworkEngine::HasAnyConsoleOutput()
	{
		for (DWORD stdHandle : {STD_OUTPUT_HANDLE, STD_ERROR_HANDLE})
		{
			HANDLE h = GetStdHandle(stdHandle);
			if (h != INVALID_HANDLE_VALUE && h != nullptr)
			{
				if (GetFileType(h) == FILE_TYPE_CHAR)
				{
					return true;
				}
			}
		}

		return false;
	}
}
