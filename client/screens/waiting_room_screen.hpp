#pragma once

#include "common/user_label.hpp"
#include "generate_sprite.hpp"
#include "screen.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <vector>

class WaitingRoomScreen : public Screen {
private:
  std::unique_ptr<sf::Texture> backgroundTexture;
  std::optional<sf::Sprite> backgroundSprite;

  std::vector<std::unique_ptr<sf::Texture>> moduleTextures;
  std::vector<sf::Sprite> modules;

  std::vector<UserLabel> playerLabels;
  bool isReady = false;
  bool loaded = false;

  void rebuildPlayerLabels(const AppContext &ctx);

public:
  WaitingRoomScreen();

  bool isLoaded() const override { return loaded; }
  void onEnter(AppContext &ctx) override;
  void handleEvent(const sf::Event &event, AppContext &ctx) override;
  void update(AppContext &ctx) override;
  void draw(sf::RenderWindow &window) override;

  std::vector<const sf::Sprite *>
  getActiveButtons(const AppContext &ctx) const override;
};
