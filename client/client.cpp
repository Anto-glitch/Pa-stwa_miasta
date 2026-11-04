#include "screens/image_sprite.hpp"
#include "systems/socket_connection.hpp"
#include "systems/socket_message.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>
#include <vector>

using namespace std;

int main()
{
    enum GameState{
    START_SCREEN,
    WAITING_ROOM,
    GAME,
    ELECTION,
    RESULTS,
    };

    sf::RenderWindow window(sf::VideoMode({800, 600}), "Client Window");

    vector<sf::Texture> textures;
    vector<sf::Sprite> screens;
    const vector<string> screenFiles = {
        "../assets/startScreen.png",
        "../assets/gameScreen.png"
    };

    if (!generateScreenSpritesFromFiles(
            screenFiles, textures, screens, {0.f, 0.f}, {800.f, 600.f})) {
        std::cerr << "Failed to load screen images." << std::endl;
        return 1;
    }

    vector<sf::Texture> moduleTextures;
    vector<sf::Sprite> modules;

    if (!generateOtherSpritesFromFiles(
            moduleTextures, modules)) {
        std::cerr << "Failed to load interface modules." << std::endl;
        return 1;
    }

    //todo : add function to load and generate sprites for other screens (waiting room, game, election, results)
    
    const int socket_id = create_socket();
    if (socket_id == -1)
        return 1;
    if (!create_connection(socket_id, 3333, "127.0.0.1")) {
        std::cerr << "Failed to connect to server." << std::endl;
        return 1;
    }

    GameState currentState = GameState::START_SCREEN;
    while (window.isOpen()) {

        while (const std::optional event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            else if (const auto* mouseEvent =
                         event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    // handle exit button or disconnect button click
                }
            }
            

            switch(currentState) {
            case GameState::START_SCREEN:
                //start screen
                //set only events crucial for each state, to avoid unnecessary event handling
                if (const auto* mouseEvent =
                         event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseEvent->button == sf::Mouse::Button::Left) 
                    {
                    // add code to handle connect button click)
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
        }


        window.clear(sf::Color::Black);

        switch(currentState) {
        case GameState::START_SCREEN:
            window.draw(screens[0]);
            window.draw(modules[0]); // Draw the exit button
            break;
        case GameState::WAITING_ROOM:
            //window.draw(screens[]); //
            break;      
        case GameState::GAME:
            window.draw(screens[1]);
            break;
        case GameState::ELECTION:
            // render election screen
            break;
        case GameState::RESULTS:
            // render results screen
            break;
        }
        
        window.display();
    }


    close_connection(socket_id);
    return 0;
}
