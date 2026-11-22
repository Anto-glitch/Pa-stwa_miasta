#pragma once

#include "../systems/network_client.hpp"
#include "common/textbox.h"

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class GameState {
  START_SCREEN,
  WAITING_ROOM,
  WAITING_ROOM_READY,
  GAME,
  ELECTION,
  RESULTS,
};

constexpr sf::Color FONTCOLOR(0xd2, 0xc9, 0xa5);

struct AppContext {
  sf::RenderWindow &window;
  sf::Font &font;
  int &socketId;
  NetworkClient &netClient;
  GameState &currentState;

  char gameLetter = '\0';
  std::string currentPrompt;
  std::vector<std::string> playerNicknames;
};

class Screen {
public:
  virtual ~Screen() = default;

  virtual bool isLoaded() const { return true; }
  virtual void onEnter(AppContext &ctx) {}
  virtual void handleEvent(const sf::Event &event, AppContext &ctx) = 0;
  virtual void handleServerMessage(const std::string &msg, AppContext &ctx) {}
  virtual void update(AppContext &ctx) {}
  virtual void draw(sf::RenderWindow &window) = 0;

  virtual std::vector<const sf::Sprite *>
  getActiveButtons(const AppContext &ctx) const {
    return {};
  }

  virtual const TextBox *getActiveTextBox(const AppContext &ctx) const {
    return nullptr;
  }
};
