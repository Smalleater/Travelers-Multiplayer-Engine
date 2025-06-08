#include <SFML/Graphics.hpp>

#include "Client.h"
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 200, 200 }), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    if (tme::Client::start("127.0.0.1", "2004"))
    {
        std::cout << "Client start succeful" << std::endl;
    }
    else
    {
        std::cout << "Client start failed" << std::endl;
    }

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
}