#include "image_sprite.hpp"
#include "socket_connection.hpp"
#include "socket_write.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>

enum GameState{
    START_SCREEN,
    WAITING_ROOM,
    GAME,
    ELECTION,
    RESULTS,
};



int main()
{

    sf::RenderWindow window(sf::VideoMode({800, 600}), "Client Window");

    sf::Texture startScreenTexture;
    const auto startScreen = generateImageSprite(
        startScreenTexture,
        "../assets/startScreen.png",
        {0.f, 0.f},
        {800.f, 600.f}
    );

    if (!startScreen) {
        std::cerr << "Failed to load start screen image." << std::endl;
        return 1;
    }

    const int socket_id = create_socket();
    if (socket_id == -1)
        return 1;
    if (!create_connection(socket_id, 3333, "127.0.0.1")) {
        std::cerr << "Failed to connect to server." << std::endl;
        return 1;
    }

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            else if (const auto* keyEvent =
                         event->getIf<sf::Event::KeyReleased>()) {
                if (keyEvent->code == sf::Keyboard::Key::Enter) {
                    // Obsługa klawisza Enter
                }
            }
            

             switch(GameState::START_SCREEN) {
            case GameState::START_SCREEN:
                // Obsługa ekranu startowego
                break;
            case GameState::WAITING_ROOM:
                // Obsługa poczekalni
                break;
            case GameState::GAME:
                // Obsługa gry
                break;
            case GameState::ELECTION:
                // Obsługa wyborów
                break;
            case GameState::RESULTS:
                // Obsługa wyników
                break;
        }



        }

        switch(GameState::START_SCREEN) {
            case GameState::START_SCREEN:
                //start screen
                if (const auto* mouseEvent =
                         event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    // Obsługa lewego przycisku myszy
                }
                }
                break;
            case GameState::WAITING_ROOM:
                if (const auto* mouseEvent =
                         event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    // Obsługa lewego przycisku myszy
                }
                }
                break;
            case GameState::GAME:
                if (const auto* mouseEvent =
                         event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    // Obsługa lewego przycisku myszy
                }
                }else if (event->is<sf::Event::TextEntered>()) {
                    // Obsługa wpisywania tekstu
                }
                else if (const auto* keyEvent =
                         event->getIf<sf::Event::KeyReleased>()) {
                if (keyEvent->code == sf::Keyboard::Key::Enter) {
                    // Obsługa klawisza Enter
                }
                }
                break;
            case GameState::ELECTION:
                // Obsługa wyborów
                 if (const auto* mouseEvent =
                         event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    // Obsługa lewego przycisku myszy
                }
                }
                break;
            case GameState::RESULTS:
                // Obsługa wyników
                break;
        }

        window.clear(sf::Color::Black);
        window.draw(*startScreen);
        window.display();
    }


    close_connection(socket_id);
    return 0;
}
