#include <iostream>

#include "TME/Server.h"
#include "TME/ServiceLocator.h"
#include "TME/ThreadManager.h"

int main()
{
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
			std::cout << "1 -> Hello World from a thread\n";
		});

	tme::ServiceLocator::threadManager().addJob([]
		{
			std::cout << "2 -> Hello World from a thread\n";
		});

	std::this_thread::sleep_for(std::chrono::seconds(1));

	tme::ServiceLocator::threadManager().stop();
	tme::ServiceLocator::reset();
}