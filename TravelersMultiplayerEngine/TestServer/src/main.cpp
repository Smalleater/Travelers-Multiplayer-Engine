#include <thread>
#include <chrono>

#include "TME/server/server.hpp"

using namespace tme;
using namespace tme::server;

int main() {
	ErrorCode ec;

	ec = Server::Get()->Start(2025);
	if (ec != ErrorCode::Success) return -1;

	std::this_thread::sleep_for(std::chrono::seconds(1));

	ec = Server::Get()->Stop();
	if (ec != ErrorCode::Success) return -1;

	return 0;
}