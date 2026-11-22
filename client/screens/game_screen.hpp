#pragma once

#include "common/textbox.h"
#include "generate_sprite.hpp"
#include "screen.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <string>

class GameScreen : public Screen {
private:
  std::unique_ptr<sf::Texture> backgroundTexture;
  std::optional<sf::Sprite> backgroundSprite;

  TextBox answerTextBox;
  sf::Text letterAnnouncementText;
  sf::Text letterText;
  sf::Text promptText;

  char displayedLetter = '\0';
  std::string displayedPrompt;
  bool loaded = false;

  void syncTexts(const AppContext &ctx);

public:
  explicit GameScreen(const sf::Font &font);

  bool isLoaded() const override { return loaded; }
  void onEnter(AppContext &ctx) override;
  void handleEvent(const sf::Event &event, AppContext &ctx) override;
  void update(AppContext &ctx) override;
  void draw(sf::RenderWindow &window) override;

  const TextBox *getActiveTextBox(const AppContext &ctx) const override;
};
