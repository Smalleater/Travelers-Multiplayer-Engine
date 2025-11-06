#include <thread>
#include <chrono>

#include "TME/debugUtils.hpp"

#include "TME/engine/networkEngine.hpp"
#include "TME/engine/iNetworkComponent.hpp"

#define ENTITY_COUNT 1000000

using namespace tme;
using namespace tme::engine;

struct ComponentTest : INetworkComponent
{
	int value;
};

int main() {
	srand(static_cast<unsigned int>(time(0)));

	ErrorCode ec;

	std::unique_ptr<NetworkEngine> networkEngine = std::make_unique<NetworkEngine>();

	std::vector<EntityId> entityCreated;
	std::vector<EntityId> entityToDestroy;

	std::shared_ptr<ComponentTest> component;

	for (size_t i = 0; i < ENTITY_COUNT; i++)
	{
		entityCreated.push_back(networkEngine->createEntity());

		component = std::make_shared<ComponentTest>();
		component->value = rand();

		ec = networkEngine->addComponentToEntity(entityCreated[i], component);
		if (ec == ErrorCode::EntityAlreadyHasComponent)
		{
			TME_ERROR_LOG("Entity %llu already has ComponentTest", entityCreated[i]);
		}
		else if (ec != ErrorCode::Success)
		{
			TME_ERROR_LOG("Failed to add ComponentTest to entity %llu. ErrorCode: %d", entityCreated[i], static_cast<int>(ec));
		}
	}

	entityToDestroy = entityCreated;
	entityCreated.clear();

	while (true)
	{
		networkEngine->beginUpdate();

		for (size_t i = 0; i < ENTITY_COUNT; i++)
		{
			networkEngine->destroyEntity(entityToDestroy[i]);
		}

		entityToDestroy.clear();

		for (size_t i = 0; i < ENTITY_COUNT; i++)
		{
			entityCreated.push_back(networkEngine->createEntity());

			component = std::make_shared<ComponentTest>();
			component->value = rand();

			ec = networkEngine->addComponentToEntity(entityCreated[i], component);
			if (ec == ErrorCode::EntityAlreadyHasComponent)
			{
				TME_ERROR_LOG("Entity %llu already has ComponentTest", entityCreated[i]);
			}
			else if (ec != ErrorCode::Success)
			{
				TME_ERROR_LOG("Failed to add ComponentTest to entity %llu. ErrorCode: %d", entityCreated[i], static_cast<int>(ec));
			}
		}

		entityToDestroy = entityCreated;
		entityCreated.clear();

		networkEngine->endUpdate();
	}

	return 0;
}