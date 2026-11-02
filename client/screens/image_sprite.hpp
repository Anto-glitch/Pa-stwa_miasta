#pragma once

#include <SFML/Graphics.hpp>

#include <optional>
#include <string>

std::optional<sf::Sprite> generateImageSprite(
    sf::Texture& texture,
    const std::string& filename,
    const sf::Vector2f& position = {0.f, 0.f},
    const sf::Vector2f& size = {100.f, 100.f});
