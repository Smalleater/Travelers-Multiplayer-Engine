#include <iostream>

#include "TME/message.hpp"

DECLARE_MESSAGE(MessageTest, std::string str;)

int main()
{
    tme::MessageTest message;
    message.str = "Hello world";

    std::cout << message.getType() << std::endl;
}