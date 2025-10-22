#include <iostream>

#include "TME/engine/message.hpp"

DECLARE_MESSAGE_BEGIN(PlayerMove)
    FIELD(int, playerId)
    FIELD(float, x)
    FIELD(float, y)
    FIELD(std::string, direction)
DECLARE_MESSAGE_END()

int main()
{
    tme::PlayerMove message;
    message.playerId = 1;
    message.x = 2;
    message.y = 3;
    message.direction = "Hello world";

    std::cout << "Message before serialization" << std::endl;
    std::cout << "1: " << message.playerId << std::endl;
    std::cout << "2: " << message.x << std::endl;
    std::cout << "3: " << message.y << std::endl;
    std::cout << "4: " << message.direction << std::endl;

    std::vector<uint8_t> msgSerialized = message.serialize();
    std::unique_ptr<tme::Message> newMessage = message.createFromBytes(msgSerialized);

    std::unique_ptr<tme::PlayerMove> newPlayerMove(static_cast<tme::PlayerMove*>(newMessage.release()));

    std::cout << "Message after serialization and createFromBytes" << std::endl;
    std::cout << "1: " << newPlayerMove->playerId << std::endl;
    std::cout << "2: " << newPlayerMove->x << std::endl;
    std::cout << "3: " << newPlayerMove->y << std::endl;
    std::cout << "4: " << newPlayerMove->direction << std::endl;

    system("pause");
}