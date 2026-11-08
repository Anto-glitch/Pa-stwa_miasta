#include "screens/generate_sprite.hpp"
#include "screens/common/exit_button.hpp"
#include "systems/socket_connection.hpp"
#include "systems/socket_message.hpp"
#include "systems/network_client.hpp" 
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
    enum GameState {
        START_SCREEN,
        WAITING_ROOM,
        GAME,
        ELECTION,
        RESULTS,
    };

    sf::RenderWindow window(sf::VideoMode({800, 600}), "Client Window");

    // textures for screens
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

    // textures for modules
    vector<std::unique_ptr<sf::Texture>> moduleTextures;
    vector<sf::Sprite> modules;

    if (!generateModuleSpritesFromFiles(moduleTextures, modules)) {
        std::cerr << "Failed to load interface modules." << std::endl;
        return 1;
    }

    // create font
    sf::Font font = createFont();

    // set textboxes
    TextBox nicknameTextBox(font, 24, sf::Color::White, {280.f, 425.f}, {220.f, 35.f});
    TextBox answerTextBox(font, 24, sf::Color::White, {280.f, 300.f}, {250.f, 35.f});

    // create texts
    char gameLetter = '\0';
    map<string, sf::Text> texts;
    texts.insert({"Nickname", createText(" ", font, 24, sf::Color::White, {280.f, 440.f})});
    texts.insert({"Letter", createText("", font, 28, sf::Color::White, {30.f, 20.f})});
    texts.insert({"Prompt", createText("", font, 24, sf::Color::White, {30.f, 80.f})});

    // create socket & NetworkClient
    int socket_id = create_socket();
    if (socket_id == -1)
        return 1;

    NetworkClient netClient;  //Utworzenie obiektu naszego klienta 

    // create cursors
    auto arrowCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);
    auto handCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
    auto textCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Text);
    if (!arrowCursor || !handCursor || !textCursor) {
        disconnect_connection(socket_id);
        return 1;
    }

    CursorType currentCursor = CursorType::Arrow;
    GameState currentState = GameState::START_SCREEN;

    while (window.isOpen()) {

      //Sprawdzanie skrzynki pocztowej co klatke 
        string serverMsg;
        while (netClient.pollMessage(serverMsg)) {
            std::cout << "Odebrano z serwera: " << serverMsg << std::endl;

            if (serverMsg == "EXIT") {
                currentState = GameState::RESULTS;
            } else {
                texts.at("Prompt").setString(serverMsg);
                currentState = GameState::GAME;
                answerTextBox.setActive(true);
            }
        }

        while (const std::optional event = window.pollEvent()) {

            // window closed
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // handle exit button click
            else if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
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

            switch (currentState) {
            case GameState::START_SCREEN:
                if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseEvent->button == sf::Mouse::Button::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        sf::Vector2f mousePosF(mousePos);

                        if (modules[0].getGlobalBounds().contains(mousePosF)) {
                            if (socket_id == -1) {
                                socket_id = create_socket();
                            }
                            if (!create_connection(socket_id, 3333, "127.0.0.1")) {
                                std::cerr << "Failed to connect to server." << std::endl;
                                disconnect_connection(socket_id);
                                break;
                            } else {
                                std::cout << "Polaczono z serwerem!" << std::endl;
                                currentState = GameState::WAITING_ROOM;

                                //Odbior wylosowanej litery (serwer wysyla 1 znak bez \n)
                                char received_letter = '\0';
                                ssize_t bytes = read_from_socket(socket_id, &received_letter, 1);
                                if (bytes > 0) {
                                    gameLetter = received_letter;
                                    texts.at("Letter").setString(string("Wylosowana litera: ") + gameLetter);
                                }

                                // Uruchomienie watku nasluchujacego i wyslanie nicku
                                netClient.startListening(socket_id);

                                string playerNick = nicknameTextBox.getText();
                                if (playerNick.empty()) {
                                    playerNick = "Gracz";
                                }
                                netClient.sendLine(playerNick);
                            }
                        }

                        if (nicknameTextBox.getGlobalBounds().contains(mousePosF)) {
                            nicknameTextBox.setActive(true);
                        } else {
                            nicknameTextBox.setActive(false);
                        }
                    }
                }
                if (const auto* textEvent = event->getIf<sf::Event::TextEntered>())
                    nicknameTextBox.typedOn(*textEvent);
                break;

            case GameState::WAITING_ROOM:
                break;

            case GameState::GAME:
                if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseEvent->button == sf::Mouse::Button::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        sf::Vector2f mousePosF(mousePos);
                        if (answerTextBox.getGlobalBounds().contains(mousePosF)) {
                            answerTextBox.setActive(true);
                        } else {
                            answerTextBox.setActive(false);
                        }
                    }
                }
                else if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                    if (textEvent->unicode != 13 && textEvent->unicode != 10) {
                        answerTextBox.typedOn(*textEvent);
                    }
                }
                else if (const auto* keyEvent = event->getIf<sf::Event::KeyReleased>()) {
                    if (keyEvent->code == sf::Keyboard::Key::Enter) {
                        string answer = answerTextBox.getText();
                        if (!answer.empty()) {
                            //Odpowiedz serwera odbierze pollMessage na gorze petli
                            netClient.sendLine(answer);
                            answerTextBox.clear();
                            answerTextBox.setActive(true);
                        }
                    }
                }
                break;

            case GameState::ELECTION:
                break;

            case GameState::RESULTS:
                break;
            }
        }

        handleCursorInteractions(
            window,
             vector<sf::Sprite>(modules.begin(), modules.end() - 1), //placeholder na text nie powinien być traktowany jak button
            nicknameTextBox,
            *arrowCursor,
            *handCursor,
            *textCursor,
            currentCursor
        );

        window.clear(sf::Color::Black);

        switch (currentState) {
        case GameState::START_SCREEN:
            window.draw(screens[0]);
            window.draw(modules[0]);
            window.draw(modules[2]);
            nicknameTextBox.draw(window);
            break;
        case GameState::WAITING_ROOM:
            window.draw(screens[1]);
            if (gameLetter != '\0') {
                window.draw(texts.at("Letter"));
            }
            break;
        case GameState::GAME:
            window.draw(screens[1]);
            if (gameLetter != '\0') {
                window.draw(texts.at("Letter"));
            }
            window.draw(texts.at("Prompt"));
            answerTextBox.draw(window);
            break;
        case GameState::ELECTION:
            break;
        case GameState::RESULTS:
            window.draw(screens[1]);
            break;
        }

        window.draw(modules[1]);
        window.display();
    }

    disconnect_connection(socket_id);
    return 0;
}