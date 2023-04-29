#include "gems.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(801, 900), "GEMS");
    GameBoard game;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::X) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                game.touchBoard(window);
            }
        }
        game.drawInter(window);
    }
}