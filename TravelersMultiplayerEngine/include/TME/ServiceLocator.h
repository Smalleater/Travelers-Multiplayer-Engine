#pragma once

#include <memory>

#include "IThreadManager.h"

namespace tme
{
	class ServiceLocator
	{
	public:
		static void provideThreadManager(std::shared_ptr<IThreadManager> manager);
		static IThreadManager& threadManager();
		static void reset();

	private:
		static std::shared_ptr<IThreadManager> m_threadManager;
	};
}
