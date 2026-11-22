#include "screens/common/cursor.hpp"
#include "screens/common/exit_button.hpp"
#include "screens/common/text.hpp"
#include "screens/election_screen.hpp"
#include "screens/game_screen.hpp"
#include "screens/generate_sprite.hpp"
#include "screens/results_screen.hpp"
#include "screens/screen.hpp"
#include "screens/start_screen.hpp"
#include "screens/waiting_room_screen.hpp"
#include "systems/network_client.hpp"
#include "systems/socket_connection.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using namespace std;

int main() {
  sf::RenderWindow window(sf::VideoMode({800, 600}), "Client Window");

  // Wspolny przycisk wyjscia widoczny na wszystkich ekranach
  vector<std::unique_ptr<sf::Texture>> commonTextures;
  vector<sf::Sprite> commonModules;
  const vector<Module> commonModuleFiles = {
      {"../assets/graphics/exitbutton.png", {660.f, 10.f}, {130.f, 65.f}},
  };
  if (!generateModuleSpritesFromFiles(commonModuleFiles, commonTextures,
                                      commonModules)) {
    std::cerr << "Failed to load exit button module." << std::endl;
    return 1;
  }
  const sf::Sprite &exitButton = commonModules[0];

  // Czcionka
  sf::Font font = createFont();

  // Siec i stan gry
  int socket_id = -1;
  NetworkClient netClient;
  GameState currentState = GameState::START_SCREEN;

  AppContext ctx{window, font, socket_id, netClient, currentState};

  // Ekrany gry
  map<GameState, std::unique_ptr<Screen>> screens;
  screens[GameState::START_SCREEN] = std::make_unique<StartScreen>(font);
  screens[GameState::WAITING_ROOM] = std::make_unique<WaitingRoomScreen>();
  screens[GameState::GAME] = std::make_unique<GameScreen>(font);
  screens[GameState::ELECTION] = std::make_unique<ElectionScreen>();
  screens[GameState::RESULTS] = std::make_unique<ResultsScreen>();

  for (const auto &[state, screen] : screens) {
    if (!screen->isLoaded()) {
      std::cerr << "Failed to load screen resources." << std::endl;
      return 1;
    }
  }

  auto getActiveScreen = [&](GameState state) -> Screen * {
    if (state == GameState::WAITING_ROOM_READY) {
      return screens.at(GameState::WAITING_ROOM).get();
    }
    return screens.at(state).get();
  };

  // Kursory
  auto arrowCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);
  auto handCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
  auto textCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Text);
  if (!textCursor) {
    std::cerr << "Failed to load text cursor." << std::endl;
  }
  window.setMouseCursorVisible(true);
  window.setMouseCursor(*arrowCursor);

  CursorType currentCursor = CursorType::Arrow;
  GameState previousState = currentState;

  auto syncScreenTransition = [&]() {
    if (currentState != previousState) {
      Screen *prevScreen = getActiveScreen(previousState);
      Screen *nextScreen = getActiveScreen(currentState);
      previousState = currentState;
      if (nextScreen != prevScreen && nextScreen != nullptr) {
        nextScreen->onEnter(ctx);
      }
    }
  };

  string serverMsg = "";
  while (window.isOpen()) {
    // 1. Odbior wiadomosci z serwera
    while (netClient.pollMessage(serverMsg)) {
      std::cout << "Odebrano z serwera: " << serverMsg << std::endl;

      if (serverMsg == "EXIT") {
        currentState = GameState::RESULTS;
      } else if (serverMsg == "START") {
        currentState = GameState::GAME;
      } else if (serverMsg.rfind("LETTER ", 0) == 0 && serverMsg.size() == 8) {
        ctx.gameLetter = serverMsg.back();
      } else {
        ctx.currentPrompt = serverMsg;
      }

      syncScreenTransition();
      getActiveScreen(currentState)->handleServerMessage(serverMsg, ctx);
    }

    // 2. Obsluga zdarzen okna i aktywnego ekranu
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
        continue;
      }

      if (const auto *mouseEvent =
              event->getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
          if (handleExitButton(*event, window, exitButton, socket_id,
                               currentState == GameState::START_SCREEN)) {
            if (currentState != GameState::START_SCREEN) {
              currentState = GameState::START_SCREEN;
              ctx.playerNicknames.clear();
              ctx.gameLetter = '\0';
              ctx.currentPrompt.clear();
              syncScreenTransition();
            }
            continue;
          }
        }
      }

      getActiveScreen(currentState)->handleEvent(*event, ctx);
      syncScreenTransition();
    }

    // 3. Aktualizacja stanu i kursora
    Screen *activeScreen = getActiveScreen(currentState);
    activeScreen->update(ctx);


    //tu do aktywnych przycisków dodajemy jeszcze exit button
    vector<const sf::Sprite *> activeButtons =
        activeScreen->getActiveButtons(ctx);
    activeButtons.push_back(&exitButton);

    handleCursorInteractions(window, activeButtons,
                             activeScreen->getActiveTextBox(ctx), *arrowCursor,
                             *handCursor, *textCursor, currentCursor);

    // 4. Rysowanie aktywnego ekranu + wspolnego przycisku wyjscia
    window.clear(sf::Color::Black);
    activeScreen->draw(window);
    window.draw(exitButton);
    window.display();
  }

  disconnect_connection(socket_id);
  return 0;
}
