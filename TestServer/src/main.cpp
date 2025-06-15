#include <iostream>

#include "TME/Server.h"

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

	while (true)
	{

	}
}