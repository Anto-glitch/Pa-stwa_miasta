#include "screens/common/cursor.hpp"
#include "screens/common/exit_button.hpp"
#include "screens/common/text.hpp"
#include "screens/common/textbox.h"
#include "screens/common/user_label.hpp"
#include "screens/generate_sprite.hpp"
#include "systems/network_client.hpp"
#include "systems/socket_connection.hpp"
#include "systems/socket_message.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using namespace std;

int main() {
  enum GameState {
    START_SCREEN,
    WAITING_ROOM,
    WAITING_ROOM_READY,
    GAME,
    ELECTION,
    RESULTS,
  };

  constexpr sf::Color FONTCOLOR(0xd2, 0xc9, 0xa5);

  sf::RenderWindow window(sf::VideoMode({800, 600}), "Client Window");

  // textures for screens
  vector<std::unique_ptr<sf::Texture>> textures;
  vector<sf::Sprite> screens;
  const vector<string> screenFiles = {
      "../assets/graphics/startScreen.png",
      "../assets/graphics/waitingroomScreen.png",
      "../assets/graphics/gameScreen.png"};

  if (!generateScreenSpritesFromFiles(screenFiles, textures, screens,
                                      {0.f, 0.f}, {800.f, 600.f})) {
    std::cerr << "Failed to load screen images." << std::endl;
    return 1;
  }

  // textures for modules (buttons)
  vector<std::unique_ptr<sf::Texture>> moduleTextures;
  vector<sf::Sprite> modules;
  const vector<Module> moduleFiles = {
      {"../assets/graphics/button.png", {300.f, 495.f}, {180.f, 80.f}},
      {"../assets/graphics/exitbutton.png", {660.f, 10.f}, {130.f, 65.f}},
  };

  if (!generateModuleSpritesFromFiles(moduleFiles, moduleTextures, modules)) {
    std::cerr << "Failed to load interface modules." << std::endl;
    return 1;
  }

  // create font
  sf::Font font = createFont();

  // set textboxes
  TextBox nicknameTextBox(font, 36, FONTCOLOR, {280.f, 425.f}, {220.f, 35.f});
  TextBox answerTextBox(font, 36, FONTCOLOR, {280.f, 425.f}, {250.f, 35.f});
  TextBox inactiveTextBox(
      font, 1, FONTCOLOR, {-1000.f, -1000.f},
      {0.f, 0.f}); // TODO: Czystsze zapisanie tego kodu, prawdopodobnie wymaga
                   // edycji handleCursorInteractions()

  // lista etykiet graczy w poczekalni
  vector<UserLabel> playerLabels; //// TODO jak dodać nowy nickname:
                                  ///playerLabels.emplace_back(nick, font,
                                  ///sf::Vector2f{250.f, 220.f + i * 85.f});

  // create texts
  char gameLetter = '\0';
  map<string, sf::Text> texts;
  texts.insert(
      {"Nickname", createText(" ", font, 24, FONTCOLOR, {280.f, 440.f})});
  texts.insert(
      {"LetterAnnouncement",
       createText("Wylosowana litera: ", font, 22, FONTCOLOR, {30.f, 110.f})});
  texts.insert({"Letter", createText("", font, 48, FONTCOLOR, {30.f, 140.f})});
  texts.insert({"Prompt", createText("", font, 24, FONTCOLOR, {30.f, 200.f})});

  // create socket & NetworkClient
  int socket_id = -1;
  NetworkClient netClient;

  // create cursors
  auto arrowCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);
  auto handCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
  auto textCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Text);
  if (!textCursor) {
    std::cerr << "Failed to load text cursor." << std::endl;
  }
  window.setMouseCursorVisible(true);
  window.setMouseCursor(*arrowCursor);

  CursorType currentCursor = CursorType::Arrow;
  GameState currentState = GameState::START_SCREEN;

  string serverMsg="";
  while (window.isOpen()) {

    // Sprawdzanie skrzynki pocztowej co klatke
  
    while (netClient.pollMessage(serverMsg)) {
      std::cout << "Odebrano z serwera: " << serverMsg << std::endl;

      if (serverMsg == "EXIT") {
        currentState = GameState::RESULTS;
      } else if(serverMsg == "START") {
        currentState = GameState::GAME;
        answerTextBox.setActive(true);

      } else if (serverMsg.rfind("LETTER ", 0) == 0 &&
                 serverMsg.size() == 8) {
        gameLetter = serverMsg.back();
        texts.at("Letter").setString(string(1, gameLetter));

      } else {
        // Nie wiem czy to potrzebne na tą chwilę ale zostawie do przemyslenia
        texts.at("Prompt").setString(serverMsg);
        // Jezeli jestesmy w WAITING_ROOM, zapisujemy pytanie w tle,
        // ale czekamy na klikniecie przycisku przed przejsciem do GAME
        if (currentState == GameState::GAME) {
          answerTextBox.setActive(true);
        }
      }
    }

    while (const std::optional event = window.pollEvent()) {

      // window closed
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }

      // handle exit button click
      else if (const auto *mouseEvent =
                   event->getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
          if (handleExitButton(*event, window, modules[1], socket_id,
                               currentState == GameState::START_SCREEN)) {
            if (currentState != GameState::START_SCREEN) {
              currentState = GameState::START_SCREEN;
              playerLabels.clear();
            }
          }
        }
      }

      switch (currentState) {
      case GameState::START_SCREEN:
        if (const auto *mouseEvent =
                event->getIf<sf::Event::MouseButtonPressed>()) {
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
                socket_id = -1;
                break;
              } else {
                std::cout << "Polaczono z serwerem!" << std::endl;

                constexpr char expectedCheck[] = "CHECK\n";
                constexpr size_t expectedCheckSize = sizeof(expectedCheck) - 1;
                char checkBuffer[expectedCheckSize];
                size_t checkBytes = 0;
                while (checkBytes < expectedCheckSize) {
                  const ssize_t bytesRead = read_from_socket(
                      socket_id, checkBuffer + checkBytes,
                      expectedCheckSize - checkBytes);
                  if (bytesRead <= 0) {
                    break;
                  }
                  checkBytes += static_cast<size_t>(bytesRead);
                }
                if (checkBytes != expectedCheckSize ||
                    string(checkBuffer, checkBytes) != expectedCheck) {
                  std::cerr << "Serwer nie potwierdzil gotowosci." << std::endl;
                  disconnect_connection(socket_id);
                  break;
                }

                string playerNick = nicknameTextBox.getText();
                if (playerNick.empty()) {
                  playerNick = "Gracz";
                }

                // Utworzenie UserLabel dla gracza w poczekalni
                playerLabels.clear();
                playerLabels.emplace_back(playerNick, font,
                                          sf::Vector2f{250.f, 270.f},
                                          sf::Vector2f{300.f, 80.f});

                currentState = GameState::WAITING_ROOM;
                nicknameTextBox.setActive(false);

                // Uruchomienie watku nasluchujacego i wyslanie nicku
                netClient.startListening(socket_id);
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
        if (const auto *textEvent = event->getIf<sf::Event::TextEntered>())
          nicknameTextBox.typedOn(*textEvent);
        break;

      case GameState::WAITING_ROOM:

        if (const auto *mouseEvent =
                event->getIf<sf::Event::MouseButtonPressed>()) {
          if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(mousePos);

            // Klikniecie przycisku w poczekalni przechodzi do wlasciwej gry
            if (modules[0].getGlobalBounds().contains(mousePosF)) {
              currentState = GameState::WAITING_ROOM_READY;
              answerTextBox.setActive(true);
              netClient.sendLine("READY");
            }
          }
        }
        break;
        case GameState::WAITING_ROOM_READY:
        break;

      case GameState::GAME:
        if (const auto *mouseEvent =
                event->getIf<sf::Event::MouseButtonPressed>()) {
          if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(mousePos);
            if (answerTextBox.getGlobalBounds().contains(mousePosF)) {
              answerTextBox.setActive(true);
            } else {
              answerTextBox.setActive(false);
            }
          }
        } else if (const auto *textEvent =
                       event->getIf<sf::Event::TextEntered>()) {
          if (textEvent->unicode != 13 && textEvent->unicode != 10) {
            answerTextBox.typedOn(*textEvent);
          }
        } else if (const auto *keyEvent =
                       event->getIf<sf::Event::KeyReleased>()) {
          if (keyEvent->code == sf::Keyboard::Key::Enter) {
            string answer = answerTextBox.getText();
            if (!answer.empty()) {
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

    // Na START_SCREEN oraz WAITING_ROOM aktywne sa oba przyciski (modules[0] +
    // exit)
    vector<sf::Sprite> activeButtons =
        (currentState == GameState::START_SCREEN ||
         currentState == GameState::WAITING_ROOM)
            ? modules
            : vector<sf::Sprite>{modules[1]};

    TextBox &activeTextBox =
        (currentState == GameState::START_SCREEN)
            ? nicknameTextBox
            : ((currentState == GameState::GAME)
                   ? answerTextBox // TODO: Czystsze zapisanie tego kodu,
                                   // prawdopodobnie wymaga edycji
                                   // handleCursorInteractions()
                   : inactiveTextBox);

    handleCursorInteractions(window, activeButtons, activeTextBox, *arrowCursor,
                             *handCursor, *textCursor, currentCursor);

    window.clear(sf::Color::Black);

    switch (currentState) {
    case GameState::START_SCREEN:
      window.draw(screens[0]);
      window.draw(modules[0]);
      nicknameTextBox.draw(window);
      break;
    case GameState::WAITING_ROOM:
      window.draw(screens[1]);
      for (const auto &label : playerLabels) {
        label.draw(window);
      }
      window.draw(modules[0]);
      break;
    case GameState::WAITING_ROOM_READY:
      window.draw(screens[1]);
      for (const auto &label : playerLabels) {
        label.draw(window);
      }
      break;
    case GameState::GAME:
      window.draw(screens[2]);
      if (gameLetter != '\0') {
        window.draw(texts.at("LetterAnnouncement"));
        window.draw(texts.at("Letter"));
      }
      window.draw(texts.at("Prompt"));
      answerTextBox.draw(window);
      break;
    case GameState::ELECTION:
      break;
    case GameState::RESULTS:
      window.draw(screens[2]);
      break;
    }

    window.draw(modules[1]);
    window.display();
  }

  disconnect_connection(socket_id);
  return 0;
}