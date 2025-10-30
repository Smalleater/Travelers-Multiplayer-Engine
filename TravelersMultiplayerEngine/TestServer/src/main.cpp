#include <thread>
#include <chrono>

#include "TME/server/server.hpp"

using namespace tme;
using namespace tme::server;

int main() {
	ErrorCode ec;

	ec = Server::Get()->Start(2025);
	if (ec != ErrorCode::Success) return -1;

	while (Server::Get()->isRunning())
	{

	}

	ec = Server::Get()->Stop();
	if (ec != ErrorCode::Success) return -1;

	return 0;
}