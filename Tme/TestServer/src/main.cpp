#include <iostream>

#include "TME/Engine.h"
#include "TME/Server.h"
#include "TME/ServiceLocator.h"

int main()
{
	tme::Engine::start();

	tme::Server::start("2004");

	tme::Engine::stop();
}