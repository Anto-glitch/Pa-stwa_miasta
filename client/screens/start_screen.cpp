#include "start_screen.hpp"

#include "../systems/socket_connection.hpp"
#include "../systems/socket_message.hpp"

#include <iostream>
#include <string>

StartScreen::StartScreen(const sf::Font &font)
    : backgroundTexture(std::make_unique<sf::Texture>()),
      nicknameTextBox(font, 36, FONTCOLOR, {280.f, 425.f}, {220.f, 35.f}) {
  backgroundSprite = generateImageSprite(
      *backgroundTexture, "../assets/graphics/startScreen.png", {0.f, 0.f},
      {800.f, 600.f});

  const std::vector<Module> moduleFiles = {
      {"../assets/graphics/button.png", {300.f, 495.f}, {180.f, 80.f}},
  };

  loaded = backgroundSprite.has_value() &&
           generateModuleSpritesFromFiles(moduleFiles, moduleTextures, modules);
}

void StartScreen::handleEvent(const sf::Event &event, AppContext &ctx) {
  if (const auto *mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
    if (mouseEvent->button == sf::Mouse::Button::Left) {
      sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);
      sf::Vector2f mousePosF(mousePos);

      if (!modules.empty() && modules[0].getGlobalBounds().contains(mousePosF)) {
        if (ctx.socketId == -1) {
          ctx.socketId = create_socket();
        }
        if (!create_connection(ctx.socketId, 3333, "127.0.0.1")) {
          std::cerr << "Failed to connect to server." << std::endl;
          disconnect_connection(ctx.socketId);
          ctx.socketId = -1;
          return;
        }

        std::cout << "Polaczono z serwerem!" << std::endl;

        constexpr char expectedCheck[] = "CHECK\n";
        constexpr size_t expectedCheckSize = sizeof(expectedCheck) - 1;
        char checkBuffer[expectedCheckSize];
        size_t checkBytes = 0;
        while (checkBytes < expectedCheckSize) {
          const ssize_t bytesRead = read_from_socket(
              ctx.socketId, checkBuffer + checkBytes,
              expectedCheckSize - checkBytes);
          if (bytesRead <= 0) {
            break;
          }
          checkBytes += static_cast<size_t>(bytesRead);
        }
        if (checkBytes != expectedCheckSize ||
            std::string(checkBuffer, checkBytes) != expectedCheck) {
          std::cerr << "Serwer nie potwierdzil gotowosci." << std::endl;
          disconnect_connection(ctx.socketId);
          return;
        }

        std::string playerNick = nicknameTextBox.getText();
        if (playerNick.empty()) {
          playerNick = "Gracz";
        }

        ctx.playerNicknames.clear();
        ctx.playerNicknames.push_back(playerNick);

        ctx.currentState = GameState::WAITING_ROOM;
        nicknameTextBox.setActive(false);

        // Uruchomienie watku nasluchujacego i wyslanie nicku
        ctx.netClient.startListening(ctx.socketId);
        ctx.netClient.sendLine(playerNick);
      }

      if (nicknameTextBox.getGlobalBounds().contains(mousePosF)) {
        nicknameTextBox.setActive(true);
      } else {
        nicknameTextBox.setActive(false);
      }
    }
  }

  if (const auto *textEvent = event.getIf<sf::Event::TextEntered>()) {
    nicknameTextBox.typedOn(*textEvent);
  }
}

void StartScreen::draw(sf::RenderWindow &window) {
  if (backgroundSprite) {
    window.draw(*backgroundSprite);
  }
  if (!modules.empty()) {
    window.draw(modules[0]);
  }
  nicknameTextBox.draw(window);
}

std::vector<const sf::Sprite *>
StartScreen::getActiveButtons(const AppContext &ctx) const {
  (void)ctx;
  if (!modules.empty()) {
    return {&modules[0]};
  }
  return {};
}

const TextBox *StartScreen::getActiveTextBox(const AppContext &ctx) const {
  (void)ctx;
  return &nicknameTextBox;
}
