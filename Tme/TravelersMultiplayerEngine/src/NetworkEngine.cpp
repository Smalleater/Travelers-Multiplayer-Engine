#include "TME/NetworkEngine.h"

#include <Windows.h>

#include "ServiceManager.h"
#include "ServiceLocator.h"

namespace tme
{
	bool NetworkEngine::m_initialized = false;

	ErrorCodes NetworkEngine::Init()
	{
		ErrorCodes result;

		result = ServiceManager::Init();
		if (result != ErrorCodes::Success)
		{
			ServiceLocator::logger().Log("Service initialization failed");
			return result;
		}

		m_initialized = true;

		ServiceLocator::logger().Log("Network engine has been started successfully");
		return ErrorCodes::Success;
	}

	ErrorCodes NetworkEngine::Shutdown()
	{
		m_initialized = false;

		ServiceLocator::threadManager().Shutdown();
		ServiceLocator::reset();

		return ErrorCodes::Success;
	}

	ErrorCodes NetworkEngine::EnsureInitialized()
	{
		if (m_initialized)
		{
			return ErrorCodes::Success;
		}

		return Init();
	}

	bool NetworkEngine::IsInitialized()
	{
		return m_initialized;
	}
}
