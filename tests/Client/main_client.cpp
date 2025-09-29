#include <iostream>
#include <thread>
#include <chrono>

#include "TME/message.hpp"

DECLARE_MESSAGE(MessageTest, std::string str;)

int main()
{
    tme::MessageTest message;
    message.str = "Hello world";
}