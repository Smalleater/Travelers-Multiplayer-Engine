#include "TME/NetworkEngine.h"

#include <Windows.h>

#include "ServiceManager.h"
#include "ServiceLocator.h"
#include "NetworkManager.h"

namespace tme
{

	bool NetworkEngine::m_initialized = false;
	void* NetworkEngine::m_networkManager = nullptr;

	ErrorCodes NetworkEngine::Init()
	{
		ErrorCodes result;

		result = ServiceManager::Init();
		if (result != ErrorCodes::Success)
		{
			ServiceLocator::logger().LogError("Service initialization failed");
			return result;
		}

		m_networkManager = new NetworkManager();

		m_initialized = true;

		ServiceLocator::logger().Log("Network engine has been started successfully");
		return ErrorCodes::Success;
	}

	ErrorCodes NetworkEngine::Shutdown()
	{
		ErrorCodes result = ErrorCodes::Success;

		if (ServiceManager::ShutDown() != ErrorCodes::Success)
		{
			result = ErrorCodes::PartialSuccess;
		}

		delete static_cast<NetworkManager*>(m_networkManager);
		m_networkManager = nullptr;

		m_initialized = false;

		return result;
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

	ErrorCodes NetworkEngine::StartServer()
	{
		if (m_networkManager == nullptr)
		{
			return ErrorCodes::NetworkEngineNotInitialized;
		}

		return static_cast<NetworkManager*>(m_networkManager)->StartServer();
	}

	ErrorCodes NetworkEngine::StartClient()
	{
		if (m_networkManager == nullptr)
		{
			return ErrorCodes::NetworkEngineNotInitialized;
		}

		return static_cast<NetworkManager*>(m_networkManager)->StartClient();
	}
}
