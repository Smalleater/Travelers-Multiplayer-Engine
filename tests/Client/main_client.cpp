#include <iostream>

#include "TME/message.hpp"

DECLARE_MESSAGE_BEGIN(PlayerMove)
    FIELD(int, playerId)
    FIELD(float, x)
    FIELD(float, y)
    FIELD(std::string, direction)
DECLARE_MESSAGE_END()

int main()
{
    tme::PlayerMove message;
    message.direction = "Hello world";

    std::cout << message.getType() << std::endl;
}