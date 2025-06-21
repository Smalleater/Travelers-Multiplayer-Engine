#pragma once

#include <memory>

#include "IThreadManager.h"
#include "ILogger.h"

namespace tme
{
	class ServiceLocator
	{
	public:
		static void provideThreadManager(std::shared_ptr<IThreadManager> manager);
		static IThreadManager& threadManager();

		static void provideLogger(std::shared_ptr<ILogger> logger);
		static ILogger& logger();

		static void reset();

	private:
		static std::shared_ptr<IThreadManager> m_threadManager;
		static std::shared_ptr<ILogger> m_logger;
	};
}
