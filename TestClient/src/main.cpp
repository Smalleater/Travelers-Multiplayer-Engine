#include <thread>
#include <chrono>

#include "TRA/client/client.hpp"
#include "TRA/engine/message.hpp"

using namespace tra;
using namespace tra::engine;
using namespace tra::client;

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
			message::HelloWorld* helloMessage = static_cast<message::HelloWorld*>(message.get());
			std::cout << "Received from server: " << helloMessage->string << std::endl;
		}

		std::shared_ptr<message::HelloWorld> message = std::make_shared<message::HelloWorld>();
		message->string = "Hello World from client!";
		Client::Get()->sendTcpMessage(message);

		Client::Get()->endUpdate();

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	/*ec = Client::Get()->Disconnect();
	if (ec != ErrorCode::Success) return -1;*/

	return 0;
}