#include "results_screen.hpp"

ResultsScreen::ResultsScreen()
    : backgroundTexture(std::make_unique<sf::Texture>()) {
  backgroundSprite = generateImageSprite(
      *backgroundTexture, "../assets/graphics/gameScreen.png", {0.f, 0.f},
      {800.f, 600.f});
  loaded = backgroundSprite.has_value();
}

void ResultsScreen::handleEvent(const sf::Event &event, AppContext &ctx) {
  (void)event;
  (void)ctx;
}

void ResultsScreen::draw(sf::RenderWindow &window) {
  if (backgroundSprite) {
    window.draw(*backgroundSprite);
  }
}
