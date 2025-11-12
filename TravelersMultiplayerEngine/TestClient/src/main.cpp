#include <thread>
#include <chrono>

#include "TME/client/client.hpp"

using namespace tme;
using namespace tme::client;

int main() {
	
	ErrorCode ec;

	ec = Client::Get()->ConnectTo("127.0.0.1", 2025);
	if (ec != ErrorCode::Success) return -1;

	while (true)
	{

	}

	ec = Client::Get()->Disconnect();
	if (ec != ErrorCode::Success) return -1;

	return 0;
}