#include <thread>
#include <chrono>

#include "TME/debugUtils.hpp"

#include "TME/server/server.hpp"

using namespace tme;
using namespace tme::server;

int main() {
	ErrorCode ec;

	ec = Server::Get()->Start(2025);
	if (ec != ErrorCode::Success) return -1;

	while (Server::Get()->isRunning())
	{
		Server::Get()->beginUpdate();
		Server::Get()->endUpdate();
	}

	ec = Server::Get()->Stop();
	if (ec != ErrorCode::Success) return -1;

	return 0;
}