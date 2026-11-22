#pragma once

#include "common/textbox.h"
#include "generate_sprite.hpp"
#include "screen.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <vector>

class StartScreen : public Screen {
private:
  std::unique_ptr<sf::Texture> backgroundTexture;
  std::optional<sf::Sprite> backgroundSprite;

  std::vector<std::unique_ptr<sf::Texture>> moduleTextures;
  std::vector<sf::Sprite> modules;

  TextBox nicknameTextBox;
  bool loaded = false;

public:
  explicit StartScreen(const sf::Font &font);

  bool isLoaded() const override { return loaded; }
  void handleEvent(const sf::Event &event, AppContext &ctx) override;
  void draw(sf::RenderWindow &window) override;

  std::vector<const sf::Sprite *>
  getActiveButtons(const AppContext &ctx) const override;
  const TextBox *getActiveTextBox(const AppContext &ctx) const override;
};
