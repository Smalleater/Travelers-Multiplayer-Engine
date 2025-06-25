#include <iostream>

#include "TME/Engine.h"
#include "TME/Server.h"
#include "TME/ServiceLocator.h"
#include "TME/ThreadManager.h"

int main()
{
	tme::Engine::start();

	if (tme::Server::start("2004"))
	{
		std::cout << "Server start succeful" << std::endl;
	}
	else
	{
		std::cout << "Server start failed" << std::endl;
	}

	std::shared_ptr<tme::ThreadManager> threadManager = std::make_shared<tme::ThreadManager>();
	threadManager->start();
	tme::ServiceLocator::provideThreadManager(threadManager);

	tme::ServiceLocator::threadManager().addJob([]
		{
			tme::ServiceLocator::logger().logInfo("Hello world from logger 01");
		});

	tme::ServiceLocator::threadManager().addJob([]
		{
			tme::ServiceLocator::logger().logWarning("Hello world from logger 02");
		});

	tme::ServiceLocator::threadManager().addJob([]
		{
			tme::ServiceLocator::logger().logError("Hello world from logger 03");
		});

	std::this_thread::sleep_for(std::chrono::seconds(1));

	tme::ServiceLocator::threadManager().stop();
	tme::ServiceLocator::reset();

	tme::Engine::stop();
}