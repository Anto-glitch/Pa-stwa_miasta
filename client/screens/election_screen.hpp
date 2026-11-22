#pragma once

#include "screen.hpp"

class ElectionScreen : public Screen {
public:
  void handleEvent(const sf::Event &event, AppContext &ctx) override;
  void draw(sf::RenderWindow &window) override;
};
