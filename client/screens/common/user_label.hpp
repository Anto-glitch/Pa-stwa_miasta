#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <string>

class UserLabel {
private:
  std::shared_ptr<sf::Texture> texture;
  sf::Sprite sprite;
  sf::Text nicknameText;
  sf::Vector2f labelSize;

  static std::shared_ptr<sf::Texture> getSharedTexture() {
    static std::shared_ptr<sf::Texture> sharedTex = nullptr;
    if (!sharedTex) {
      sharedTex = std::make_shared<sf::Texture>();
      if (!sharedTex->loadFromFile("../assets/graphics/user_label.png")) {
        std::cerr << "Failed to load ../assets/graphics/user_label.png"
                  << std::endl;
      }
    }
    return sharedTex;
  }

  void updateTextPosition() {
    sf::Vector2f pos = sprite.getPosition();
    sf::FloatRect textBounds = nicknameText.getLocalBounds();

    // Przesunięcie w prawo (za ikonę awatara) i wyśrodkowanie w pionie
    float textX = pos.x + labelSize.x * 0.30f;
    float textY = pos.y + (labelSize.y - textBounds.size.y) / 2.f -
                  textBounds.position.y;

    nicknameText.setPosition({textX, textY});
  }

public:
  UserLabel(const std::string &nickname, const sf::Font &font,
            sf::Vector2f position = {250.f, 270.f},
            sf::Vector2f size = {300.f, 80.f}, unsigned int charSize = 32,
            sf::Color textColor = sf::Color(0x57, 0x4c, 0x53))
      : texture(getSharedTexture()), sprite(*texture),
        nicknameText(font, nickname, charSize), labelSize(size) {

    sf::Vector2u texSize = texture->getSize();
    if (texSize.x > 0 && texSize.y > 0) {
      sprite.setScale({size.x / static_cast<float>(texSize.x),
                       size.y / static_cast<float>(texSize.y)});
    }

    sprite.setPosition(position);
    nicknameText.setFillColor(textColor);
    updateTextPosition();
  }

  void setPosition(sf::Vector2f position) {
    sprite.setPosition(position);
    updateTextPosition();
  }

  void setNickname(const std::string &nickname) {
    nicknameText.setString(nickname);
    updateTextPosition();
  }

  void draw(sf::RenderWindow &window) const {
    window.draw(sprite);
    window.draw(nicknameText);
  }
};