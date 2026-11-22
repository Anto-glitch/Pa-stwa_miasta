#pragma once

#include "generate_sprite.hpp"
#include "screen.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>

class ResultsScreen : public Screen {
private:
  std::unique_ptr<sf::Texture> backgroundTexture;
  std::optional<sf::Sprite> backgroundSprite;
  bool loaded = false;

public:
  ResultsScreen();

  bool isLoaded() const override { return loaded; }
  void handleEvent(const sf::Event &event, AppContext &ctx) override;
  void draw(sf::RenderWindow &window) override;
};
