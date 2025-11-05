#include <thread>
#include <chrono>

#include "TME/debugUtils.hpp"

#include "TME/engine/networkEngine.hpp"

using namespace tme;
using namespace tme::engine;

struct ComponentTest : INetworkComponent
{
	int value;
};

int main() {
	ErrorCode ec;

	std::unique_ptr<NetworkEngine> networkEngine = std::make_unique<NetworkEngine>();

	EntityId entityTest = networkEngine->createEntity();
	TME_INFO_LOG("Created entity with ID: %llu", entityTest);

	std::shared_ptr component = std::make_shared<ComponentTest>();
	component->value = 333;

	ec = networkEngine->addComponentToEntity(entityTest, component);
	if (ec == ErrorCode::Success)
	{
		TME_INFO_LOG("Added ComponentTest to entity %llu with value %d", entityTest, component->value);
	}
	else if (ec == ErrorCode::EntityAlreadyHasComponent)
	{
		TME_ERROR_LOG("Entity %llu already has ComponentTest", entityTest);
	}
	else
	{
		TME_ERROR_LOG("Failed to add ComponentTest to entity %llu. ErrorCode: %d", entityTest, static_cast<int>(ec));
	}

	return 0;
}