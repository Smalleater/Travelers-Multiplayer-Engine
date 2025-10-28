#include <thread>
#include <chrono>

#include "TME/client/client.hpp"

using namespace tme;
using namespace tme::client;

int main() {
	
	ErrorCode ec;

	ec = Client::Get()->ConnectTo("127.0.0.1", 2025);
	if (ec != ErrorCode::Success) return -1;

	std::this_thread::sleep_for(std::chrono::seconds(1));

	return 0;
}