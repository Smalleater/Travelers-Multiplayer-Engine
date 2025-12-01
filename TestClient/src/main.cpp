#include <thread>
#include <chrono>

#include "TME/client/client.hpp"
#include "TME/engine/message.hpp"

using namespace tme;
using namespace tme::engine;
using namespace tme::client;

DECLARE_MESSAGE_BEGIN(HelloWorld)
FIELD(std::string, string)
DECLARE_MESSAGE_END()

int main() {
	
	ErrorCode ec;

	ec = Client::Get()->ConnectTo("127.0.0.1", 2025);
	if (ec != ErrorCode::Success) return -1;

	while (Client::Get()->IsConnected())
	{
		Client::Get()->beginUpdate();

		auto getMessageResult = Client::Get()->getTcpMessages("HelloWorld");
		for (auto message : getMessageResult.second)
		{
			HelloWorld* helloMessage = static_cast<HelloWorld*>(message.get());
			std::cout << "Received from server: " << helloMessage->string << std::endl;
		}

		std::shared_ptr<HelloWorld> message = std::make_shared<HelloWorld>();
		message->string = "Hello World from client!";
		Client::Get()->sendTcpMessage(message);

		Client::Get()->endUpdate();

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	/*ec = Client::Get()->Disconnect();
	if (ec != ErrorCode::Success) return -1;*/

	return 0;
}