#include "TME/core/wsaInitializer.hpp"

#ifdef _WIN32

namespace tme::core
{
	WSAInitializer* WSAInitializer::m_singleton = nullptr;

	WSAInitializer::WSAInitializer() : m_wsaData{}
	{

	}

	WSAInitializer::~WSAInitializer()
	{
		CleanUp();
	}

	WSAInitializer* WSAInitializer::Get()
	{
		if (m_singleton == nullptr)
		{
			m_singleton = new WSAInitializer;
		}

		return m_singleton;
	}

	ErrorCode WSAInitializer::Init()
	{
		int iResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
		if (iResult != 0)
		{
			return ErrorCode::WSAStartupFailed;
		}

		return ErrorCode::Success;
	}

	void WSAInitializer::CleanUp()
	{
		WSACleanup();
	}
}

#endif