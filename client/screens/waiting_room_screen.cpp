#include "waiting_room_screen.hpp"

WaitingRoomScreen::WaitingRoomScreen()
    : backgroundTexture(std::make_unique<sf::Texture>()) {
  backgroundSprite = generateImageSprite(
      *backgroundTexture, "../assets/graphics/waitingroomScreen.png",
      {0.f, 0.f}, {800.f, 600.f});

  const std::vector<Module> moduleFiles = {
      {"../assets/graphics/button.png", {300.f, 495.f}, {180.f, 80.f}},
  };

  loaded = backgroundSprite.has_value() &&
           generateModuleSpritesFromFiles(moduleFiles, moduleTextures, modules);
}

void WaitingRoomScreen::rebuildPlayerLabels(const AppContext &ctx) {
  playerLabels.clear();
  for (size_t i = 0; i < ctx.playerNicknames.size(); ++i) {
    playerLabels.emplace_back(ctx.playerNicknames[i], ctx.font,
                              sf::Vector2f{250.f, 270.f + i * 85.f},
                              sf::Vector2f{300.f, 80.f});
  }
}

void WaitingRoomScreen::onEnter(AppContext &ctx) {
  isReady = (ctx.currentState == GameState::WAITING_ROOM_READY);
  rebuildPlayerLabels(ctx);
}

void WaitingRoomScreen::handleEvent(const sf::Event &event, AppContext &ctx) {
  if (ctx.currentState != GameState::WAITING_ROOM) {
    return;
  }

  if (const auto *mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
    if (mouseEvent->button == sf::Mouse::Button::Left) {
      sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);
      sf::Vector2f mousePosF(mousePos);

      if (!modules.empty() && modules[0].getGlobalBounds().contains(mousePosF)) {
        ctx.currentState = GameState::WAITING_ROOM_READY;
        isReady = true;
        ctx.netClient.sendLine("READY");
      }
    }
  }
}

void WaitingRoomScreen::update(AppContext &ctx) {
  isReady = (ctx.currentState == GameState::WAITING_ROOM_READY);
  if (playerLabels.size() != ctx.playerNicknames.size()) {
    rebuildPlayerLabels(ctx);
  }
}

void WaitingRoomScreen::draw(sf::RenderWindow &window) {
  if (backgroundSprite) {
    window.draw(*backgroundSprite);
  }
  for (const auto &label : playerLabels) {
    label.draw(window);
  }
  if (!isReady && !modules.empty()) {
    window.draw(modules[0]);
  }
}

std::vector<const sf::Sprite *>
WaitingRoomScreen::getActiveButtons(const AppContext &ctx) const {
  if (ctx.currentState == GameState::WAITING_ROOM && !modules.empty()) {
    return {&modules[0]};
  }
  return {};
}
