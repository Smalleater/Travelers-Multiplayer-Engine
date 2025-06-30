#include <iostream>

#include "TME/Engine.h"
#include "TME/Server.h"
#include "TME/ServiceLocator.h"

int main()
{
	tme::Engine::start();

	if (tme::Server::start("2004"))
	{
		tme::ServiceLocator::logger().logInfo("Server start succeful");
	}
	else
	{
		tme::ServiceLocator::logger().logError("Server start failed");
	}

	tme::Engine::stop();
}