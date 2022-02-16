#include "emul8.hpp"

int main()
{
    sf::RenderWindow main_window(sf::VideoMode(1280, 640), "Emul-8");
    sf::Font font;
    font.loadFromFile("../resources/Roboto-Black.ttf");

    sf::Text text("Hello world!", font);
    text.setCharacterSize(50);

    main_window.draw(text);
    main_window.display();

    while (main_window.isOpen())
    {
        sf::Event event;
        while (main_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                main_window.close();
        }
    }
}
