#include "screens/generate_sprite.hpp"
#include "screens/common/exit_button.hpp"
#include "systems/socket_connection.hpp"
#include "systems/socket_message.hpp"
#include "screens/common/text.hpp"
#include "screens/common/textbox.h"
#include "screens/common/cursor.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <optional>
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>

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



    //textures for screens
    vector<std::unique_ptr<sf::Texture>> textures;
    vector<sf::Sprite> screens;
    const vector<string> screenFiles = {
        "../assets/graphics/startScreen.png",
        "../assets/graphics/gameScreen.png"
    };

    if (!generateScreenSpritesFromFiles(
            screenFiles, textures, screens, {0.f, 0.f}, {800.f, 600.f})) {
        std::cerr << "Failed to load screen images." << std::endl;
        return 1;
    }
    //textures for modules
    vector<std::unique_ptr<sf::Texture>> moduleTextures;
    vector<sf::Sprite> modules;

    if (!generateModuleSpritesFromFiles(
            moduleTextures, modules)) {
        std::cerr << "Failed to load interface modules." << std::endl;
        return 1;
    }

    //todo : add function to load and generate sprites for other screens (waiting room, game, election, results)

    //create font
    sf::Font font = createFont();

    //set textboxes
    TextBox nicknameTextBox(font, 24, sf::Color::White, {50.f, 50.f});

    //create texts
    map<string, sf::Text> texts;
    texts.insert({"Nickname", createText(" ", font, 50, sf::Color::White, {280.f, 440.f})});

    //create socket
    int socket_id = create_socket();
    if (socket_id == -1)
        return 1;
    //create cursors
    auto arrowCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);
    if (!arrowCursor) {
        std::cerr << "Failed to load arrow cursor." << std::endl;
    }
    auto handCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
    if (!handCursor) {
        std::cerr << "Failed to load hand cursor." << std::endl;
    }
    auto textCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Text);
    if (!textCursor) {
        std::cerr << "Failed to load text cursor." << std::endl;
    }    
        window.setMouseCursorVisible(true);
        window.setMouseCursor(*arrowCursor);

    CursorType currentCursor = CursorType::Arrow;
    GameState currentState = GameState::START_SCREEN;
    while (window.isOpen()) {

        while (const std::optional event = window.pollEvent()) {

            //window closed
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            //handle exit button click
            else if (const auto* mouseEvent =
                         event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    if (handleExitButton(
                            *event,
                            window,
                            modules[1],
                            socket_id,
                            currentState == GameState::START_SCREEN)) {
                        if (currentState != GameState::START_SCREEN) {
                            currentState = GameState::START_SCREEN;
                        }
                    }
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
                        //connect button handling
                        sf::Vector2i mousePos = sf::Mouse::getPosition( window );
                        sf::Vector2f mousePosF(mousePos);
                        if(modules[0].getGlobalBounds().contains(mousePosF)) {
                            if (socket_id == -1) {
                                socket_id = create_socket();
                            }
                            if (!create_connection(socket_id, 3333, "127.0.0.1")) {
                                std::cerr << "Failed to connect to server." << std::endl;
                                disconnect_connection(socket_id);
                                break;
                            }
                            else {
                                currentState = GameState::WAITING_ROOM;
                            }
                        }
                        if(nicknameTextBox.getGlobalBounds().contains(mousePosF)) {
                            nicknameTextBox.setActive(true);
                        }
                        else {
                            nicknameTextBox.setActive(false);
                        }
                    }
                }
                if(const auto* textEvent = event->getIf<sf::Event::TextEntered>()) 
                        nicknameTextBox.typedOn(*textEvent);
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

        handleCursorInteractions(
            window,
            modules,
            nicknameTextBox,
            *arrowCursor,
            *handCursor,
            *textCursor,
            currentCursor
        );

        window.clear(sf::Color::Black);

        switch(currentState) {
        case GameState::START_SCREEN:
            window.draw(screens[0]);
            window.draw(modules[0]);
            window.draw(modules[2]);
            nicknameTextBox.draw(window);
            break;
        case GameState::WAITING_ROOM:
            window.draw(screens[1]); //
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
        //draw exit button on all screens
        window.draw(modules[1]);

        window.display();
    }


    disconnect_connection(socket_id);
    return 0;
}
